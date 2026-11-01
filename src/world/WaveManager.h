#pragma once
// ====================================================================
// FILE:     src/world/WaveManager.h
// Spawns 3 enemy waves of increasing difficulty in random lanes.
// ====================================================================

#include <vector>
#include <memory>
#include <string>
#include "../entities/Enemy.h"

class WaveManager {
public:
    static WaveManager& instance();

    void reset();
    void update(float dt, std::vector<std::shared_ptr<Enemy>>& enemies);

    int getCurrentWave() const { return m_currentWave; }
    int getTotalWaves() const { return 3; }
    int getEnemiesRemaining() const { return m_enemiesRemainingInWave + m_aliveEnemiesCount; }
    std::string getWaveName() const;

    bool allWavesFinished() const { return m_allWavesCleared; }
    bool isBetweenWaves() const { return m_betweenWavesTimer > 0.0f; }

private:
    WaveManager() = default;

    int m_currentWave = 1;
    int m_enemiesSpawnedInWave = 0;
    int m_enemiesRemainingInWave = 0;
    int m_aliveEnemiesCount = 0;

    float m_spawnTimer = 0.0f;
    float m_betweenWavesTimer = 0.0f;
    bool m_allWavesCleared = false;
};
