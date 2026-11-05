// ====================================================================
// FILE:     src/world/WaveManager.cpp
// OWNER:    Member 1
// TASKS:    M1-06, M1-07
// STATUS:   IMPLEMENTED
// ====================================================================

#include "WaveManager.h"
#include "../entities/Enemy.h"
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
    UnitType unitType = UnitType::EasyEnemy;
    switch (type) {
        case EntityType::ENEMY_EASY:
            unitType = UnitType::EasyEnemy;
            break;
        case EntityType::ENEMY_MEDIUM:
            unitType = UnitType::MediumEnemy;
            break;
        case EntityType::ENEMY_HARD:
            unitType = UnitType::HardEnemy;
            break;
        default:
            return nullptr;
    }

    Enemy* enemy = new Enemy();
    enemy->init(unitType, lane, Constants::ENEMY_SPAWN_X);
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
