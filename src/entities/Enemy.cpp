#include "Enemy.h"
// ====================================================================
// FILE:     src/entities/Enemy.cpp
// OWNER:    Member 2
// TASKS:    M2-02
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   The three enemy types (Easy upper-ranged, Medium lower-ranged,
//   Hard melee) with their stats and behaviour.
//
// RESPONSIBILITIES:
//   - Health: Easy 5, Medium 5, Hard 7
//   - Damage 1 per attack, Hard deals 2
//   - Advance left automatically, Hard runs faster
//   - Easy fires HIGH attacks, Medium fires LOW attacks, Hard attacks in melee
//   - takeDamage(), isDead(), drops a collectible on death
//
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - update(dt)
//   - draw()
//   - takeDamage(amount)
//   - isDead()
//   - attackHeight()
//
// INTERACTS WITH: Entity, CharacterModels, Animation, WaveManager, CollisionSystem
// ====================================================================
