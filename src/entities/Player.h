#pragma once
// ====================================================================
// FILE:     src/entities/Player.h
// OWNER:    Member 2
// TASKS:    M2-02, M2-12
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   The three player types (Melee, Ranged, Defense) with their stats
//   and behaviour.
//
// RESPONSIBILITIES:
//   - Health: Melee 5, Ranged 5, Defense 7 hits
//   - Damage 1 per attack (Defense deals none)
//   - Advance right automatically (Defense stays still)
//   - Attack cooldown and attack animation trigger
//   - Jump and duck state (Defense ignores both)
//   - takeDamage(), isDead(), shielded-by-Defense flag
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - update(dt)
//   - draw()
//   - takeDamage(amount)
//   - jump()
//   - duck()
//   - isShielded()
//
// INTERACTS WITH: Entity, CharacterModels, Animation, CollisionSystem, HealthBar
// ====================================================================
