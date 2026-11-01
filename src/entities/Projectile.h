#pragma once
// ====================================================================
// FILE:     src/entities/Projectile.h
// Projectiles: Zane's Ice Shuriken (Player), Venomari's Acid (High),
// Hypnobrai's Sonic Blast (Low).
// ====================================================================

#include "Entity.h"

enum class ProjectileType {
    PLAYER_SHURIKEN,
    ENEMY_UPPER_ACID,  // High attack: avoided by ducking
    ENEMY_LOWER_SONIC  // Low attack: avoided by jumping
};

class Projectile : public Entity {
public:
    Projectile(ProjectileType projType, int laneIdx, float startX, float startY);

    void update(float dt) override;
    void draw() override;

    bool isHighAttack() const { return pType == ProjectileType::ENEMY_UPPER_ACID; }
    bool isLowAttack() const { return pType == ProjectileType::ENEMY_LOWER_SONIC; }
    bool isPlayerProjectile() const { return pType == ProjectileType::PLAYER_SHURIKEN; }

    ProjectileType pType;

private:
    float m_speed = 360.0f;
    float m_animTime = 0.0f;
};
