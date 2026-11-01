#pragma once
// ====================================================================
// FILE:     src/audio/AudioManager.h
// OWNER:    Member 4
// TASKS:    M4-11
// Audio engine: 8-bit chiptune BGM, retro sound effects, volume control.
// ====================================================================

#include <string>

enum class SoundID {
    ATTACK_SWORD,
    ATTACK_SHURIKEN,
    ATTACK_HISS,
    UNIT_HIT,
    UNIT_DEATH,
    SHARD_COLLECT,
    LANE_SELECT,
    PLAYER_PLACE,
    VICTORY,
    DEFEAT
};

class AudioManager {
public:
    static AudioManager& instance();

    bool init();
    void cleanup();

    // Music & SFX
    void playMusic(bool loop = true);
    void stopMusic();
    void playSound(SoundID id);

    // Volume & Mute (US-57)
    void setVolume(float volume); // 0.0f .. 1.0f
    float getVolume() const { return m_volume; }
    void volumeUp(float step = 0.1f);
    void volumeDown(float step = 0.1f);
    void toggleMute();
    bool isMuted() const { return m_muted; }

private:
    AudioManager();
    ~AudioManager();

    float m_volume = 0.70f;
    bool m_muted = false;
    bool m_musicPlaying = false;

    // Platform-specific internal state
    struct Impl;
    Impl* m_impl = nullptr;
};
