#define NOMINMAX
#include "AudioManager.h"
#include <windows.h>
#include <mmsystem.h>
#include <thread>
#include <atomic>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#pragma comment(lib, "winmm.lib")

namespace {
    constexpr int SAMPLE_RATE = 22050;
    constexpr int BUFFER_SAMPLES = 2048;
    constexpr int NUM_BUFFERS = 3;

    struct ActiveSFX {
        SoundID id;
        int samplePos = 0;
        int totalSamples = 0;
    };
}

struct AudioManager::Impl {
    HWAVEOUT hWaveOut = nullptr;
    WAVEHDR waveHeaders[NUM_BUFFERS];
    short audioBuffers[NUM_BUFFERS][BUFFER_SAMPLES];
    int currentBuffer = 0;

    std::thread audioThread;
    std::atomic<bool> running{false};

    std::vector<ActiveSFX> activeSounds;
    CRITICAL_SECTION soundLock;

    float bgmPhase = 0.0f;
    float bgmBassPhase = 0.0f;
    int bgmStep = 0;
    int bgmSampleCounter = 0;

    // Ninjago retro melody notes (frequencies in Hz)
    // D4, F4, G4, A4, C5, D5, A4, G4
    const float melodyNotes[16] = {
        293.66f, 0.0f, 349.23f, 392.00f, 440.00f, 523.25f, 440.00f, 392.00f,
        349.23f, 293.66f, 349.23f, 392.00f, 440.00f, 392.00f, 349.23f, 293.66f
    };
    const float bassNotes[16] = {
        146.83f, 146.83f, 174.61f, 174.61f, 196.00f, 196.00f, 220.00f, 220.00f,
        174.61f, 174.61f, 146.83f, 146.83f, 196.00f, 196.00f, 174.61f, 146.83f
    };
};

AudioManager& AudioManager::instance() {
    static AudioManager mgr;
    return mgr;
}

AudioManager::AudioManager() {
    m_impl = new Impl();
    InitializeCriticalSection(&m_impl->soundLock);
}

AudioManager::~AudioManager() {
    cleanup();
    DeleteCriticalSection(&m_impl->soundLock);
    delete m_impl;
}

bool AudioManager::init() {
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT res = waveOutOpen(&m_impl->hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    if (res != MMSYSERR_NOERROR) {
        std::cerr << "AudioManager: waveOutOpen failed (" << res << ")" << std::endl;
        return false;
    }

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        ZeroMemory(&m_impl->waveHeaders[i], sizeof(WAVEHDR));
        m_impl->waveHeaders[i].lpData = reinterpret_cast<LPSTR>(m_impl->audioBuffers[i]);
        m_impl->waveHeaders[i].dwBufferLength = BUFFER_SAMPLES * sizeof(short);
        waveOutPrepareHeader(m_impl->hWaveOut, &m_impl->waveHeaders[i], sizeof(WAVEHDR));
    }

    m_impl->running = true;
    m_impl->audioThread = std::thread([this]() {
        const int samplesPerNote = SAMPLE_RATE / 6; // 6 notes per second

        while (m_impl->running) {
            WAVEHDR& hdr = m_impl->waveHeaders[m_impl->currentBuffer];
            while (m_impl->running && (hdr.dwFlags & WHDR_INQUEUE)) {
                Sleep(2);
            }
            if (!m_impl->running) break;

            short* out = m_impl->audioBuffers[m_impl->currentBuffer];
            float effectiveVol = m_muted ? 0.0f : m_volume;

            for (int i = 0; i < BUFFER_SAMPLES; ++i) {
                float sample = 0.0f;

                // 1. Synthesize 8-bit chiptune BGM
                if (m_musicPlaying && effectiveVol > 0.001f) {
                    m_impl->bgmSampleCounter++;
                    if (m_impl->bgmSampleCounter >= samplesPerNote) {
                        m_impl->bgmSampleCounter = 0;
                        m_impl->bgmStep = (m_impl->bgmStep + 1) % 16;
                    }

                    float noteFreq = m_impl->melodyNotes[m_impl->bgmStep];
                    if (noteFreq > 0.0f) {
                        m_impl->bgmPhase += noteFreq / SAMPLE_RATE;
                        if (m_impl->bgmPhase >= 1.0f) m_impl->bgmPhase -= 1.0f;
                        // 25% duty cycle square wave (classic Game Boy sound!)
                        float square = (m_impl->bgmPhase < 0.25f) ? 0.12f : -0.12f;
                        sample += square;
                    }

                    float bassFreq = m_impl->bassNotes[m_impl->bgmStep];
                    m_impl->bgmBassPhase += bassFreq / SAMPLE_RATE;
                    if (m_impl->bgmBassPhase >= 1.0f) m_impl->bgmBassPhase -= 1.0f;
                    // Triangle wave for bass
                    float tri = (m_impl->bgmBassPhase < 0.5f)
                        ? (4.0f * m_impl->bgmBassPhase - 1.0f)
                        : (3.0f - 4.0f * m_impl->bgmBassPhase);
                    sample += tri * 0.10f;
                }

                // 2. Synthesize Active Sound Effects
                EnterCriticalSection(&m_impl->soundLock);
                for (size_t s = 0; s < m_impl->activeSounds.size();) {
                    ActiveSFX& sfx = m_impl->activeSounds[s];
                    float t = (float)sfx.samplePos / (float)SAMPLE_RATE;
                    float sfxSample = 0.0f;

                    switch (sfx.id) {
                    case SoundID::ATTACK_SWORD: {
                        // Fast downward square sweep
                        float freq = 800.0f - (t * 2200.0f);
                        if (freq < 100.0f) freq = 100.0f;
                        float p = std::fmod(t * freq, 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.25f : -0.25f) * (1.0f - t / 0.22f);
                        break;
                    }
                    case SoundID::ATTACK_SHURIKEN: {
                        // High crystalline chime
                        float freq = 1200.0f + (t * 800.0f);
                        float p = std::fmod(t * freq, 1.0f);
                        sfxSample = ((p < 0.3f) ? 0.20f : -0.20f) * (1.0f - t / 0.18f);
                        break;
                    }
                    case SoundID::ATTACK_HISS: {
                        // Noise burst for venom/snake hiss
                        float noise = ((float)(rand() % 2000) / 1000.0f) - 1.0f;
                        sfxSample = noise * 0.18f * (1.0f - t / 0.25f);
                        break;
                    }
                    case SoundID::SHARD_COLLECT: {
                        // Two-tone coin pickup ding! (B5 987Hz -> E6 1318Hz)
                        float freq = (t < 0.08f) ? 987.0f : 1318.0f;
                        float p = std::fmod(t * freq, 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.25f : -0.25f) * (1.0f - t / 0.32f);
                        break;
                    }
                    case SoundID::UNIT_HIT: {
                        float noise = ((float)(rand() % 2000) / 1000.0f) - 1.0f;
                        sfxSample = noise * 0.22f * (1.0f - t / 0.12f);
                        break;
                    }
                    case SoundID::UNIT_DEATH: {
                        float freq = 400.0f - t * 800.0f;
                        if (freq < 60.0f) freq = 60.0f;
                        float p = std::fmod(t * freq, 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.25f : -0.25f) * (1.0f - t / 0.35f);
                        break;
                    }
                    case SoundID::LANE_SELECT: {
                        float p = std::fmod(t * 700.0f, 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.15f : -0.15f) * (1.0f - t / 0.06f);
                        break;
                    }
                    case SoundID::PLAYER_PLACE: {
                        float freq = (t < 0.06f) ? 330.0f : ((t < 0.12f) ? 415.0f : 493.0f);
                        float p = std::fmod(t * freq, 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.22f : -0.22f) * (1.0f - t / 0.22f);
                        break;
                    }
                    case SoundID::VICTORY: {
                        // Victory fanfare arpeggio
                        float notes[6] = {523.25f, 659.25f, 783.99f, 1046.50f, 880.00f, 1046.50f};
                        int noteIdx = std::min(5, (int)(t / 0.15f));
                        float p = std::fmod(t * notes[noteIdx], 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.30f : -0.30f) * (1.0f - t / 1.1f);
                        break;
                    }
                    case SoundID::DEFEAT: {
                        float notes[4] = {440.0f, 415.3f, 392.0f, 349.2f};
                        int noteIdx = std::min(3, (int)(t / 0.22f));
                        float p = std::fmod(t * notes[noteIdx], 1.0f);
                        sfxSample = ((p < 0.5f) ? 0.25f : -0.25f) * (1.0f - t / 0.95f);
                        break;
                    }
                    }

                    sample += sfxSample;
                    sfx.samplePos++;
                    if (sfx.samplePos >= sfx.totalSamples) {
                        m_impl->activeSounds.erase(m_impl->activeSounds.begin() + s);
                    } else {
                        ++s;
                    }
                }
                LeaveCriticalSection(&m_impl->soundLock);

                // Master volume scaling and clipping
                sample *= effectiveVol;
                sample = std::max(-0.95f, std::min(0.95f, sample));
                out[i] = static_cast<short>(sample * 32767.0f);
            }

            waveOutWrite(m_impl->hWaveOut, &hdr, sizeof(WAVEHDR));
            m_impl->currentBuffer = (m_impl->currentBuffer + 1) % NUM_BUFFERS;
        }
    });

    playMusic(true);
    return true;
}

void AudioManager::cleanup() {
    if (m_impl->running) {
        m_impl->running = false;
        if (m_impl->audioThread.joinable()) {
            m_impl->audioThread.join();
        }
    }
    if (m_impl->hWaveOut) {
        waveOutReset(m_impl->hWaveOut);
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            waveOutUnprepareHeader(m_impl->hWaveOut, &m_impl->waveHeaders[i], sizeof(WAVEHDR));
        }
        waveOutClose(m_impl->hWaveOut);
        m_impl->hWaveOut = nullptr;
    }
}

void AudioManager::playMusic(bool) {
    m_musicPlaying = true;
}

void AudioManager::stopMusic() {
    m_musicPlaying = false;
}

void AudioManager::playSound(SoundID id) {
    if (!m_impl->running) return;

    ActiveSFX sfx;
    sfx.id = id;
    sfx.samplePos = 0;

    switch (id) {
    case SoundID::ATTACK_SWORD:    sfx.totalSamples = (int)(SAMPLE_RATE * 0.22f); break;
    case SoundID::ATTACK_SHURIKEN:  sfx.totalSamples = (int)(SAMPLE_RATE * 0.18f); break;
    case SoundID::ATTACK_HISS:      sfx.totalSamples = (int)(SAMPLE_RATE * 0.25f); break;
    case SoundID::SHARD_COLLECT:    sfx.totalSamples = (int)(SAMPLE_RATE * 0.32f); break;
    case SoundID::UNIT_HIT:         sfx.totalSamples = (int)(SAMPLE_RATE * 0.12f); break;
    case SoundID::UNIT_DEATH:       sfx.totalSamples = (int)(SAMPLE_RATE * 0.35f); break;
    case SoundID::LANE_SELECT:      sfx.totalSamples = (int)(SAMPLE_RATE * 0.06f); break;
    case SoundID::PLAYER_PLACE:     sfx.totalSamples = (int)(SAMPLE_RATE * 0.22f); break;
    case SoundID::VICTORY:          sfx.totalSamples = (int)(SAMPLE_RATE * 1.10f); break;
    case SoundID::DEFEAT:           sfx.totalSamples = (int)(SAMPLE_RATE * 0.95f); break;
    }

    EnterCriticalSection(&m_impl->soundLock);
    // Keep max 8 concurrent SFX
    if (m_impl->activeSounds.size() > 8) {
        m_impl->activeSounds.erase(m_impl->activeSounds.begin());
    }
    m_impl->activeSounds.push_back(sfx);
    LeaveCriticalSection(&m_impl->soundLock);
}

void AudioManager::setVolume(float volume) {
    m_volume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioManager::volumeUp(float step) {
    setVolume(m_volume + step);
    playSound(SoundID::LANE_SELECT);
}

void AudioManager::volumeDown(float step) {
    setVolume(m_volume - step);
    playSound(SoundID::LANE_SELECT);
}

void AudioManager::toggleMute() {
    m_muted = !m_muted;
    playSound(SoundID::LANE_SELECT);
}
