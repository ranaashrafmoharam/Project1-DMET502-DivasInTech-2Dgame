#include "WaveManager.h"
#include "../core/Constants.h"
#include <cstdlib>

WaveManager& WaveManager::instance() {
    static WaveManager wm;
    return wm;
}

void WaveManager::reset() {
    m_currentWave = 1;
    m_enemiesSpawnedInWave = 0;
    m_enemiesRemainingInWave = Constants::WAVE1_ENEMY_COUNT;
    m_aliveEnemiesCount = 0;
    m_spawnTimer = 1.0f; // first enemy spawns after 1s
    m_betweenWavesTimer = 0.0f;
    m_allWavesCleared = false;
}

std::string WaveManager::getWaveName() const {
    if (m_currentWave == 1) return "SCOUTS (VENOMARI)";
    if (m_currentWave == 2) return "THE TRIBES (VENOMARI & HYPNOBRAI)";
    if (m_currentWave == 3) return "SERPENTINE ARMY (GENERALS & BRUTES)";
    return "VICTORY";
}

void WaveManager::update(float dt, std::vector<std::shared_ptr<Enemy>>& enemies) {
    if (m_allWavesCleared) return;

    // Count alive enemies currently on screen
    m_aliveEnemiesCount = 0;
    for (const auto& e : enemies) {
        if (e && e->isAlive) m_aliveEnemiesCount++;
    }

    // Between waves break pause
    if (m_betweenWavesTimer > 0.0f) {
        m_betweenWavesTimer -= dt;
        if (m_betweenWavesTimer <= 0.0f) {
            // Start next wave
            m_currentWave++;
            m_enemiesSpawnedInWave = 0;
            if (m_currentWave == 2) {
                m_enemiesRemainingInWave = Constants::WAVE2_ENEMY_COUNT;
            } else if (m_currentWave == 3) {
                m_enemiesRemainingInWave = Constants::WAVE3_ENEMY_COUNT;
            }
            m_spawnTimer = 1.0f;
        }
        return;
    }

    // Spawning enemies inside the current wave
    if (m_enemiesRemainingInWave > 0) {
        m_spawnTimer -= dt;
        if (m_spawnTimer <= 0.0f) {
            m_spawnTimer = Constants::SPAWN_INTERVAL;

            // Pick random lane (US-34: appear at right-most edge in random lane)
            int randomLane = std::rand() % Constants::NUM_LANES;
            float spawnX = Constants::FIELD_X1 - 50.0f;

            // Determine enemy type according to wave rules (US-39)
            EntityType eType = EntityType::ENEMY_EASY;
            if (m_currentWave == 1) {
                // Wave 1: Easy only (Venomari)
                eType = EntityType::ENEMY_EASY;
            } else if (m_currentWave == 2) {
                // Wave 2: Easy and Medium mixed
                eType = (std::rand() % 2 == 0) ? EntityType::ENEMY_EASY : EntityType::ENEMY_MEDIUM;
            } else {
                // Wave 3: Easy, Medium, and Hard mixed
                int roll = std::rand() % 10;
                if (roll < 4) eType = EntityType::ENEMY_EASY;
                else if (roll < 7) eType = EntityType::ENEMY_MEDIUM;
                else eType = EntityType::ENEMY_HARD; // Hard Constrictai
            }

            enemies.push_back(std::make_shared<Enemy>(eType, randomLane, spawnX));
            m_enemiesSpawnedInWave++;
            m_enemiesRemainingInWave--;
        }
    } else {
        // All enemies of this wave have spawned!
        // Wave finishes only when all enemies on screen are cleared (spec)
        if (m_aliveEnemiesCount == 0) {
            if (m_currentWave < 3) {
                // Pause before next wave starts
                m_betweenWavesTimer = Constants::WAVE_REST_PAUSE;
            } else {
                // All 3 waves defeated! (US-49)
                m_allWavesCleared = true;
            }
        }
    }
}
