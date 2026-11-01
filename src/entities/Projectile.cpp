#include "Projectile.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"

Projectile::Projectile(ProjectileType projType, int laneIdx, float startX, float startY)
    : Entity(EntityType::PROJECTILE_PLAYER, laneIdx, startX, startY, 1), pType(projType) {
    
    width = 24.0f;
    height = 24.0f;

    switch (projType) {
    case ProjectileType::PLAYER_SHURIKEN:
        type = EntityType::PROJECTILE_PLAYER;
        m_speed = Constants::PROJECTILE_SPEED;
        // Centered along lane
        y = Constants::laneY(laneIdx) + 40.0f;
        break;
    case ProjectileType::ENEMY_UPPER_ACID:
        type = EntityType::PROJECTILE_ENEMY_UPPER;
        m_speed = -Constants::PROJECTILE_SPEED * 0.75f;
        // Upper attack: flies at head level (y + 55)
        y = Constants::laneY(laneIdx) + 55.0f;
        break;
    case ProjectileType::ENEMY_LOWER_SONIC:
        type = EntityType::PROJECTILE_ENEMY_LOWER;
        m_speed = -Constants::PROJECTILE_SPEED * 0.75f;
        // Lower attack: flies at feet / low level (y + 15)
        y = Constants::laneY(laneIdx) + 15.0f;
        break;
    }
}

void Projectile::update(float dt) {
    m_animTime += dt;
    x += m_speed * dt;

    // Out of bounds check
    if (x < Constants::FIELD_X0 - 50.0f || x > Constants::FIELD_X1 + 50.0f) {
        isAlive = false;
    }
}

void Projectile::draw() {
    Renderer& ren = Renderer::instance();

    switch (pType) {
    case ProjectileType::PLAYER_SHURIKEN: {
        // Fast spinning ice shuriken
        float rot = m_animTime * 18.0f;
        ren.drawSpriteCentered("shuriken", x + width * 0.5f, y + height * 0.5f, 26.0f, 26.0f, false, rot);
        break;
    }
    case ProjectileType::ENEMY_UPPER_ACID: {
        // Acid globule with wobble
        float rot = std::sin(m_animTime * 10.0f) * 0.4f;
        ren.drawSpriteCentered("acid", x + width * 0.5f, y + height * 0.5f, 28.0f, 28.0f, true, rot);
        break;
    }
    case ProjectileType::ENEMY_LOWER_SONIC: {
        // Sonic pulse wave ring
        ren.drawSpriteCentered("sonic", x + width * 0.5f, y + height * 0.5f, 28.0f, 28.0f, true, 0.0f);
        break;
    }
    }
}
