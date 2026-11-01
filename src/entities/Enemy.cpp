#include "Enemy.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "../ui/HealthBar.h"
#include <cmath>

Enemy::Enemy(EntityType enemyType, int laneIdx, float startX)
    : Entity(enemyType, laneIdx, startX, Constants::laneY(laneIdx) + 10.0f, 5) {
    
    width = 54.0f;
    height = 72.0f;

    switch (enemyType) {
    case EntityType::ENEMY_EASY:
        m_spriteName = "venomari";
        health = maxHealth = Constants::HEALTH_EASY_ENEMY;
        m_speed = Constants::SPEED_ENEMY_NORMAL;
        m_damage = Constants::DAMAGE_EASY_ENEMY;
        m_attackInterval = 1.70f;
        break;
    case EntityType::ENEMY_MEDIUM:
        m_spriteName = "hypnobrai";
        health = maxHealth = Constants::HEALTH_MEDIUM_ENEMY;
        m_speed = Constants::SPEED_ENEMY_NORMAL;
        m_damage = Constants::DAMAGE_MEDIUM_ENEMY;
        m_attackInterval = 1.70f;
        break;
    case EntityType::ENEMY_HARD:
        m_spriteName = "constrictai";
        health = maxHealth = Constants::HEALTH_HARD_ENEMY; // 7 lives
        m_speed = Constants::SPEED_ENEMY_FAST;             // Runs faster! (US-37)
        m_damage = Constants::DAMAGE_HARD_ENEMY;           // 2 damage per attack
        m_attackInterval = 0.85f;                          // Fast melee attacks
        width = 62.0f;
        height = 76.0f;
        break;
    default:
        m_spriteName = "venomari";
        break;
    }
}

void Enemy::resetAttackCooldown() {
    m_attackCooldown = m_attackInterval;
}

void Enemy::update(float dt) {
    m_animTime += dt;

    // Move left toward home base unless locked in melee combat
    if (!isFighting) {
        x -= m_speed * dt;
    }

    if (m_attackCooldown > 0.0f) {
        m_attackCooldown -= dt;
    }

    // Ranged enemies (Easy Venomari and Medium Hypnobrai) fire projectiles periodically
    if ((type == EntityType::ENEMY_EASY || type == EntityType::ENEMY_MEDIUM) && m_attackCooldown <= 0.0f) {
        readyToShoot = true;
    }
}

void Enemy::draw() {
    Renderer& ren = Renderer::instance();
    HealthBar& hb = HealthBar::instance();

    // Subtle slithering bobbing animation
    float bob = std::sin(m_animTime * 8.0f) * 2.5f;

    // Ground shadow
    ren.drawCircle(x + width * 0.5f, y + 2.0f, 18.0f, glm::vec4(0.04f, 0.06f, 0.10f, 0.40f));

    // Red damage tint feedback (US-43)
    float damageTint = hb.getTintAmount(health, maxHealth);
    ren.setTint(damageTint);

    // Draw enemy sprite flipped horizontally (facing left toward monastery)
    ren.drawSprite(m_spriteName, x, y + bob, width, height, true);

    // Reset tint
    ren.setTint(0.0f);

    // Visible Health Bar above unit (US-44)
    hb.draw(x, y + bob, width, height, health, maxHealth, true);
}
