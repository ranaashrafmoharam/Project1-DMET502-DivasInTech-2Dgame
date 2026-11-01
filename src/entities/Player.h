#pragma once
// ====================================================================
// FILE:     src/entities/Player.h
// Defenders: Kai (Melee), Zane (Ranged), Cole (Defense / Wall).
// ====================================================================

#include "Entity.h"
#include <string>

class Player : public Entity {
public:
    Player(EntityType playerType, int laneIdx, float startX);

    void update(float dt) override;
    void draw() override;

    void jump();
    void duck();

    bool isJumping() const { return m_jumpTimer > 0.0f; }
    bool isDucking() const { return m_duckTimer > 0.0f; }

    bool canAttack() const { return m_attackCooldown <= 0.0f; }
    void resetAttackCooldown();

    bool isFighting = false;
    bool isShielded = false;

    // Ranged attack firing flag
    bool readyToShoot = false;

private:
    float m_attackCooldown = 0.0f;
    float m_attackInterval = 1.0f;

    // Jump / Duck
    float m_jumpTimer = 0.0f;
    float m_duckTimer = 0.0f;
    float m_yOffset = 0.0f;

    // Melee attack animation
    float m_slashAnim = 0.0f;
    float m_animTime = 0.0f;
    std::string m_spriteName;
};
