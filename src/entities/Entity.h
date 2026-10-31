#pragma once
// ====================================================================
// FILE:     src/entities/Entity.h
// OWNER:    Member 1
// TASKS:    M1-02
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   Base class every unit shares. Gives Member 1's systems one
//   common way to move, collide with and draw any unit.
//
// RESPONSIBILITIES:
//   - Position, lane index, health, max health, alive flag
//   - Unit type tag (player / enemy and which kind)
//   - Virtual update(dt) and draw()
//   - Bounding box for collision
//
// INTERACTS WITH: Player, Enemy, Projectile, Collectible, CollisionSystem
// ====================================================================
