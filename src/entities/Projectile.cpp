#include "Projectile.h"
// ====================================================================
// FILE:     src/entities/Projectile.cpp
// OWNER:    Member 2
// TASKS:    M2-11
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   A shot fired by the Ranged Player or a ranged enemy.
//
// RESPONSIBILITIES:
//   - Spawn at the weapon, travel along the lane
//   - Tagged HIGH (duck) or LOW (jump)
//   - Animated in flight; disappears on collision
//   - Exposes position and hitbox for collision checks
//
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - update(dt)
//   - draw()
//   - getBounds()
//   - isHigh()
//   - destroy()
//
// INTERACTS WITH: Entity, PropModels, CollisionSystem
// ====================================================================
