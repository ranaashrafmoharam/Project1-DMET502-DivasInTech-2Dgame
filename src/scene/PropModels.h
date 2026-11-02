#pragma once
// ====================================================================
// FILE:     src/scene/PropModels.h
// OWNER:    Member 3
// TASKS:    M3-09, M3-10
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   Weapon, projectile and collectible visuals.
//
// RESPONSIBILITIES:
//   - Melee and ranged weapons
//   - Player projectile and enemy upper/lower projectiles
//   - Collectible/coin model with spin animation
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - drawMeleeWeapon()
//   - drawRangedWeapon()
//   - drawProjectile(type)
//   - drawCollectible()
//
// INTERACTS WITH: CharacterModels, Projectile, Collectible
// ====================================================================

#pragma once

class PropModels
{
public:
    void drawMeleeWeapon(float x, float y, float angle = 0.0f);
    void drawRangedWeapon(float x, float y, float angle = 0.0f);
    void drawProjectile(int type, float x, float y, float rotation = 0.0f);
    void drawCollectible(float x, float y, float spin = 0.0f);
};
