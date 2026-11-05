#pragma once
// ====================================================================
// FILE:     src/entities/Entity.h
// OWNER:    Member 1
// TASKS:    M1-02
// STATUS:   IMPLEMENTED
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

#include <glm/glm.hpp>
#include <algorithm>
#include "../core/Constants.h"

enum class EntityType {
    PLAYER_MELEE,
    PLAYER_RANGED,
    PLAYER_DEFENSE,
    ENEMY_EASY,    // Upper-ranged (attacks high -> player ducks)
    ENEMY_MEDIUM,  // Lower-ranged (attacks low -> player jumps)
    ENEMY_HARD,    // Melee brute (runs fast, 2 damage)
    PROJECTILE_PLAYER,
    PROJECTILE_HIGH,
    PROJECTILE_LOW,
    COLLECTIBLE
};

struct Bounds {
    float x;      // Top-left X
    float y;      // Top-left Y
    float width;
    float height;

    bool intersects(const Bounds& other) const {
        return (x < other.x + other.width &&
                x + width > other.x &&
                y < other.y + other.height &&
                y + height > other.y);
    }
};

class Entity {
public:
    glm::vec2 position; // Center position (x, y)
    glm::vec2 size;     // (width, height)
    int laneIndex;
    int health;
    int maxHealth;
    bool alive;
    EntityType type;

    float speed;
    bool isFighting;
    bool shielded;

    // Jump & Duck mechanics
    bool isJumping;
    bool isDucking;
    float jumpTimer;
    float duckTimer;

    // Combat timers
    float attackTimer;
    float attackCooldown;
    int attackDamage;

public:
    Entity(EntityType t, int lane, float x, float y, int hp, float spd)
        : position(x, y)
        , size(Constants::UNIT_WIDTH, Constants::UNIT_HEIGHT)
        , laneIndex(lane)
        , health(hp)
        , maxHealth(hp)
        , alive(true)
        , type(t)
        , speed(spd)
        , isFighting(false)
        , shielded(false)
        , isJumping(false)
        , isDucking(false)
        , jumpTimer(0.0f)
        , duckTimer(0.0f)
        , attackTimer(0.0f)
        , attackCooldown(Constants::ATTACK_COOLDOWN_MELEE)
        , attackDamage(1)
    {
    }

    virtual ~Entity() = default;

    virtual void update(float dt) {
        if (!alive) return;

        // Advance horizontally along the lane if not stopped in combat
        if (!isFighting) {
            if (isPlayer() && type != EntityType::PLAYER_DEFENSE) {
                position.x += speed * dt;
            } else if (isEnemy()) {
                position.x -= speed * dt;
            } else if (type == EntityType::PROJECTILE_PLAYER) {
                position.x += speed * dt;
            } else if (type == EntityType::PROJECTILE_HIGH || type == EntityType::PROJECTILE_LOW) {
                position.x -= speed * dt;
            }
        }

        if (attackTimer > 0.0f) {
            attackTimer -= dt;
        }
    }

    virtual void draw() {
        // Will be overridden by Member 2 / 3 / 4 models.
    }

    virtual void takeDamage(int amount) {
        if (!alive || shielded) return;
        health -= amount;
        if (health <= 0) {
            health = 0;
            alive = false;
        }
    }

    virtual bool isDead() const {
        return !alive || health <= 0;
    }

    virtual Bounds getBounds() const {
        float effectiveY = position.y;
        if (isJumping) {
            effectiveY += Constants::JUMP_HEIGHT_OFFSET;
        }
        float effectiveHeight = size.y;
        if (isDucking) {
            effectiveHeight *= 0.5f;
            effectiveY += size.y * 0.25f;
        }

        return Bounds{
            position.x - size.x * 0.5f,
            effectiveY - effectiveHeight * 0.5f,
            size.x,
            effectiveHeight
        };
    }

    virtual void jump() {
        if (type == EntityType::PLAYER_DEFENSE) return; // Defense ignores jump
        if (isJumping) {
            isJumping = false; // Toggle back to standing
        } else {
            isJumping = true;  // Stays jumping until toggled or ducked
            isDucking = false;
        }
    }

    virtual void duck() {
        if (type == EntityType::PLAYER_DEFENSE) return; // Defense ignores duck
        if (isDucking) {
            isDucking = false; // Toggle back to standing
        } else {
            isDucking = true;  // Stays ducking until toggled or jumped
            isJumping = false;
        }
    }

    // Type query helpers
    bool isPlayer() const {
        return type == EntityType::PLAYER_MELEE ||
               type == EntityType::PLAYER_RANGED ||
               type == EntityType::PLAYER_DEFENSE;
    }

    bool isEnemy() const {
        return type == EntityType::ENEMY_EASY ||
               type == EntityType::ENEMY_MEDIUM ||
               type == EntityType::ENEMY_HARD;
    }

    bool isDefense() const {
        return type == EntityType::PLAYER_DEFENSE;
    }

    bool isProjectile() const {
        return type == EntityType::PROJECTILE_PLAYER ||
               type == EntityType::PROJECTILE_HIGH ||
               type == EntityType::PROJECTILE_LOW;
    }
};
