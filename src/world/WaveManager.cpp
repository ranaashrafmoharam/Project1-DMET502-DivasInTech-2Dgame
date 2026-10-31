#include "WaveManager.h"
// ====================================================================
// FILE:     src/world/WaveManager.cpp
// OWNER:    Member 1
// TASKS:    M1-06, M1-07
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
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
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - update(dt)
//   - getCurrentWave()
//   - getEnemiesRemaining()
//   - allWavesFinished()
//
// INTERACTS WITH: Enemy, Game, HUD
// ====================================================================
