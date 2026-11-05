#pragma once
// ====================================================================
// FILE:     src/world/WaveManager.h
// OWNER:    Member 1
// TASKS:    M1-06, M1-07
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Spawner and wave controller.
//
// RESPONSIBILITIES:
//   - Wave 1: 3+ Easy. Wave 2: 5+ Easy/Medium. Wave 3: 10+ mixed
//   - Spawn at the right edge in a random lane on a timer
//   - Wave starts at first enemy, ends at last spawn
//   - Next wave begins only after all enemies are defeated
//   - Report wave number and enemies remaining
//
// PLANNED PUBLIC INTERFACE:
//   - update(dt, aliveEnemiesCount)
//   - getCurrentWave()
//   - getEnemiesRemaining()
//   - allWavesFinished()
// ====================================================================

#include <vector>
#include "../entities/Entity.h"

class WaveManager {
public:
    WaveManager();

    void reset();

    // Updates spawner timer; returns a newly spawned enemy, or nullptr if no spawn this frame.
    // aliveEnemiesCount is the number of active living enemies currently on the playfield.
    Entity* update(float dt, int aliveEnemiesCount);

    int getCurrentWave() const { return m_currentWave; }
    int getTotalWaves() const { return 3; }
    int getEnemiesRemaining() const { return m_enemiesRemaining; }
    bool allWavesFinished() const { return m_allWavesFinished; }
    bool isWaveSpawningDone() const { return m_spawnQueue.empty(); }

private:
    int m_currentWave;
    float m_spawnTimer;
    bool m_allWavesFinished;
    int m_enemiesRemaining;

    std::vector<EntityType> m_spawnQueue;

    void setupWave(int waveNumber);
    Entity* spawnEnemy(EntityType type);
};
