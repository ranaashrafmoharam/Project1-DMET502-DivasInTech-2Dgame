#include "Player.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include <cmath>
#include <algorithm>

Player::Player(EntityType playerType, int laneIdx, float startX)
    : Entity(playerType, laneIdx, startX, Constants::laneY(laneIdx) + 10.0f, 5) {
    
    width = 54.0f;
    height = 72.0f;

    switch (playerType) {
    case EntityType::PLAYER_MELEE:
        m_spriteName = "kai";
        health = maxHealth = Constants::HEALTH_MELEE_PLAYER;
        m_attackInterval = 0.85f;
        break;
    case EntityType::PLAYER_RANGED:
        m_spriteName = "zane";
        health = maxHealth = Constants::HEALTH_RANGED_PLAYER;
        m_attackInterval = 1.35f;
        break;
    case EntityType::PLAYER_DEFENSE:
        m_spriteName = "cole";
        health = maxHealth = Constants::HEALTH_DEFENSE_PLAYER;
        m_attackInterval = 999.0f; // does not attack
        break;
    default:
        m_spriteName = "kai";
        break;
    }
}

void Player::jump() {
    // Defense player is immune to jump/duck (spec)
    if (type == EntityType::PLAYER_DEFENSE) return;
    if (m_jumpTimer <= 0.0f && m_duckTimer <= 0.0f) {
        m_jumpTimer = Constants::JUMP_DURATION;
    }
}

void Player::duck() {
    if (type == EntityType::PLAYER_DEFENSE) return;
    if (m_jumpTimer <= 0.0f && m_duckTimer <= 0.0f) {
        m_duckTimer = Constants::DUCK_DURATION;
    }
}

void Player::resetAttackCooldown() {
    m_attackCooldown = m_attackInterval;
    m_slashAnim = 0.20f;
}

void Player::update(float dt) {
    m_animTime += dt;

    // Movement: Advances right automatically (except Defense which stays still)
    if (type != EntityType::PLAYER_DEFENSE && !isFighting) {
        x += Constants::SPEED_PLAYER * dt;
        // Clamp to field boundary
        if (x > Constants::FIELD_X1 - width) {
            x = Constants::FIELD_X1 - width;
        }
    }

    // Jump logic
    if (m_jumpTimer > 0.0f) {
        m_jumpTimer -= dt;
        float progress = 1.0f - (m_jumpTimer / Constants::JUMP_DURATION);
        m_yOffset = std::sin(progress * 3.14159f) * Constants::JUMP_HEIGHT;
    } else if (m_duckTimer > 0.0f) {
        m_duckTimer -= dt;
        m_yOffset = -8.0f;
    } else {
        m_yOffset = 0.0f;
    }

    // Attack cooldown
    if (m_attackCooldown > 0.0f) {
        m_attackCooldown -= dt;
    }

    // Ranged player shoots periodically
    if (type == EntityType::PLAYER_RANGED && m_attackCooldown <= 0.0f) {
        readyToShoot = true;
    }

    if (m_slashAnim > 0.0f) {
        m_slashAnim -= dt;
    }
}

void Player::draw() {
    Renderer& ren = Renderer::instance();

    float currentY = y + m_yOffset;
    float currentH = (m_duckTimer > 0.0f) ? height * 0.75f : height;

    // Shadow on ground
    ren.drawCircle(x + width * 0.5f, y + 2.0f, 18.0f, glm::vec4(0.04f, 0.06f, 0.10f, 0.40f));

    // Red damage tint feedback (US-43: intensifies as health drops)
    ren.setTint(0.0f);

    // Defense aura / stone glow if Cole
    if (type == EntityType::PLAYER_DEFENSE) {
        ren.drawRect(x - 4, currentY - 2, width + 8, currentH + 4, glm::vec4(0.55f, 0.40f, 0.20f, 0.35f));
    }

    // Draw defender sprite
    ren.drawSprite(m_spriteName, x, currentY, width, currentH);

    // Melee sword attack animation (Kai)
    if (type == EntityType::PLAYER_MELEE && (m_slashAnim > 0.0f || isFighting)) {
        float rot = (m_slashAnim > 0.0f) ? -0.8f : 0.2f;
        ren.drawSpriteCentered("sword", x + width + 8.0f, currentY + currentH * 0.55f, 36.0f, 36.0f, false, rot);
    }

    // Reset tint for UI
    ren.setTint(0.0f);

    // Visible Health Bar above unit (US-44)
    hb.draw(x, currentY, width, currentH, health, maxHealth, false);
}
