// ====================================================================
// FILE:     src/world/WaveManager.cpp
// OWNER:    Member 1
// TASKS:    M1-06, M1-07
// STATUS:   IMPLEMENTED
// ====================================================================

#include "WaveManager.h"
#include <cstdlib>
#include <iostream>

WaveManager::WaveManager() {
    reset();
}

void WaveManager::reset() {
    m_currentWave = 1;
    m_allWavesFinished = false;
    m_spawnTimer = 1.0f; // Brief delay before first enemy appears
    setupWave(1);
}

void WaveManager::setupWave(int waveNumber) {
    m_currentWave = waveNumber;
    m_spawnQueue.clear();

    if (waveNumber == 1) {
        // Wave 1: at least 3 Easy enemies
        for (int i = 0; i < Constants::WAVE_1_COUNT; ++i) {
            m_spawnQueue.push_back(EntityType::ENEMY_EASY);
        }
    } else if (waveNumber == 2) {
        // Wave 2: at least 5 enemies (Easy and Medium mixed)
        m_spawnQueue.push_back(EntityType::ENEMY_EASY);
        m_spawnQueue.push_back(EntityType::ENEMY_MEDIUM);
        m_spawnQueue.push_back(EntityType::ENEMY_EASY);
        m_spawnQueue.push_back(EntityType::ENEMY_MEDIUM);
        m_spawnQueue.push_back(EntityType::ENEMY_EASY);
    } else if (waveNumber == 3) {
        // Wave 3: at least 10 enemies (Easy, Medium, and Hard mixed)
        m_spawnQueue.push_back(EntityType::ENEMY_EASY);
        m_spawnQueue.push_back(EntityType::ENEMY_MEDIUM);
        m_spawnQueue.push_back(EntityType::ENEMY_HARD);
        m_spawnQueue.push_back(EntityType::ENEMY_EASY);
        m_spawnQueue.push_back(EntityType::ENEMY_MEDIUM);
        m_spawnQueue.push_back(EntityType::ENEMY_HARD);
        m_spawnQueue.push_back(EntityType::ENEMY_EASY);
        m_spawnQueue.push_back(EntityType::ENEMY_MEDIUM);
        m_spawnQueue.push_back(EntityType::ENEMY_HARD);
        m_spawnQueue.push_back(EntityType::ENEMY_HARD);
    }

    m_enemiesRemaining = static_cast<int>(m_spawnQueue.size());
    std::cout << "[WaveManager] Starting Wave " << m_currentWave << " with " << m_enemiesRemaining << " enemies." << std::endl;
}

Entity* WaveManager::spawnEnemy(EntityType type) {
    int lane = std::rand() % Constants::LANE_COUNT;
    float spawnX = Constants::ENEMY_SPAWN_X;
    float spawnY = Constants::getLaneCenterY(lane);

    Entity* enemy = nullptr;

    switch (type) {
        case EntityType::ENEMY_EASY: {
            enemy = new Entity(
                EntityType::ENEMY_EASY,
                lane,
                spawnX,
                spawnY,
                Constants::HP_ENEMY_EASY,
                Constants::SPEED_ENEMY_EASY
            );
            enemy->attackDamage = Constants::DMG_ENEMY_EASY;
            enemy->attackCooldown = Constants::ATTACK_COOLDOWN_RANGED;
            break;
        }
        case EntityType::ENEMY_MEDIUM: {
            enemy = new Entity(
                EntityType::ENEMY_MEDIUM,
                lane,
                spawnX,
                spawnY,
                Constants::HP_ENEMY_MEDIUM,
                Constants::SPEED_ENEMY_MEDIUM
            );
            enemy->attackDamage = Constants::DMG_ENEMY_MEDIUM;
            enemy->attackCooldown = Constants::ATTACK_COOLDOWN_RANGED;
            break;
        }
        case EntityType::ENEMY_HARD: {
            enemy = new Entity(
                EntityType::ENEMY_HARD,
                lane,
                spawnX,
                spawnY,
                Constants::HP_ENEMY_HARD,
                Constants::SPEED_ENEMY_HARD // Fast!
            );
            enemy->attackDamage = Constants::DMG_ENEMY_HARD; // 2 damage
            enemy->attackCooldown = Constants::ATTACK_COOLDOWN_MELEE;
            break;
        }
        default:
            break;
    }

    return enemy;
}

Entity* WaveManager::update(float dt, int aliveEnemiesCount) {
    if (m_allWavesFinished) {
        m_enemiesRemaining = 0;
        return nullptr;
    }

    m_enemiesRemaining = static_cast<int>(m_spawnQueue.size()) + aliveEnemiesCount;

    // Check if current wave is completely cleared
    if (m_spawnQueue.empty() && aliveEnemiesCount == 0) {
        if (m_currentWave < 3) {
            setupWave(m_currentWave + 1);
            m_spawnTimer = 2.0f; // Wave transition rest period
            return nullptr;
        } else {
            m_allWavesFinished = true;
            std::cout << "[WaveManager] All waves defeated! VICTORY!" << std::endl;
            return nullptr;
        }
    }

    // Spawn timer update
    if (!m_spawnQueue.empty()) {
        m_spawnTimer -= dt;
        if (m_spawnTimer <= 0.0f) {
            EntityType nextEnemy = m_spawnQueue.front();
            m_spawnQueue.erase(m_spawnQueue.begin());
            m_spawnTimer = Constants::ENEMY_SPAWN_INTERVAL;
            return spawnEnemy(nextEnemy);
        }
    }

    return nullptr;
}
