#pragma once
// ====================================================================
// FILE:     src/entities/Enemy.h
// Serpentine Enemies: Venomari (Easy), Hypnobrai (Medium), Constrictai (Hard).
// ====================================================================

#include "Entity.h"
#include <string>

class Enemy : public Entity {
public:
    Enemy(EntityType enemyType, int laneIdx, float startX);

    void update(float dt) override;
    void draw() override;

    bool canAttack() const { return m_attackCooldown <= 0.0f; }
    void resetAttackCooldown();

    int getDamage() const { return m_damage; }

    bool isFighting = false;
    bool readyToShoot = false;

private:
    float m_speed = 50.0f;
    int m_damage = 1;
    float m_attackCooldown = 0.0f;
    float m_attackInterval = 1.2f;
    float m_animTime = 0.0f;

    std::string m_spriteName;
};
