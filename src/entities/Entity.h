#pragma once
// ====================================================================
// FILE:     src/entities/Entity.h
// Base class for all gameplay units: players, enemies, projectiles,
// collectibles. Gives a unified interface for update and collision.
// ====================================================================

#include <glm/glm.hpp>

enum class EntityType {
    PLAYER_MELEE,
    PLAYER_RANGED,
    PLAYER_DEFENSE,
    ENEMY_EASY,
    ENEMY_MEDIUM,
    ENEMY_HARD,
    PROJECTILE_PLAYER,
    PROJECTILE_ENEMY_UPPER,
    PROJECTILE_ENEMY_LOWER,
    COLLECTIBLE_SHARD
};

struct Bounds {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    bool overlaps(const Bounds& o) const {
        return (x < o.x + o.w && x + w > o.x &&
                y < o.y + o.h && y + h > o.y);
    }
};

class Entity {
public:
    EntityType type;
    int lane = 0;
    float x = 0.0f;
    float y = 0.0f;
    float width = 60.0f;
    float height = 75.0f;

    int health = 5;
    int maxHealth = 5;
    bool isAlive = true;

    Entity(EntityType t, int laneIdx, float startX, float startY, int hp)
        : type(t), lane(laneIdx), x(startX), y(startY), health(hp), maxHealth(hp) {}

    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    virtual Bounds getBounds() const {
        return Bounds{x, y, width, height};
    }

    virtual void takeDamage(int amount) {
        health -= amount;
        if (health <= 0) {
            health = 0;
            isAlive = false;
        }
    }
};
