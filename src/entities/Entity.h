#pragma once
// ====================================================================
// FILE:     src/entities/Entity.h
// OWNER:    Member 1
// TASKS:    M1-02
// STATUS:   MINIMAL STUB IMPLEMENTED BY MEMBER 2 (agreed) - header only,
//           no Entity.cpp is required. Member 1 owns this file and may
//           extend it, but the members below are the agreed contract.
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

#include "../core/Math.h"
#include "../core/Constants.h"

// Which concrete unit this entity is.
enum class UnitType {
    None,
    MeleePlayer,     // Kai  - Fire Ninja
    RangedPlayer,    // Zane - Ice Ninja
    DefensePlayer,   // Cole - Earth Ninja
    EasyEnemy,       // Venomari  - upper ranged
    MediumEnemy,     // Hypnobrai - lower ranged
    HardEnemy        // Constrictai - melee brute
};

// Which side an entity fights for.
enum class Faction { None, Player, Enemy };

// Axis-aligned box used by the CollisionSystem.
struct Bounds {
    float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

    float left()    const { return x; }
    float right()   const { return x + w; }
    float bottom()  const { return y; }
    float top()     const { return y + h; }
    float centerX() const { return x + w * 0.5f; }
    float centerY() const { return y + h * 0.5f; }

    bool overlaps(const Bounds& o) const {
        return x < o.x + o.w && o.x < x + w && y < o.y + o.h && o.y < y + h;
    }
};

class Entity {
public:
    Entity() = default;
    virtual ~Entity() = default;

    // position = feet of the unit (bottom-centre) in world space.
    Vec2      position{ 0.0f, 0.0f };
    int       lane       = 0;
    float     health     = 1.0f;
    float     maxHealth  = 1.0f;
    bool      alive      = true;
    UnitType  type       = UnitType::None;
    Faction   faction    = Faction::None;
    Vec2      size{ C::CHARACTER_WIDTH, C::CHARACTER_HEIGHT };

    // ---- the two calls Member 1's systems make on every unit ----
    virtual void update(float dt) = 0;
    virtual void draw()           = 0;

    // ---- shared helpers ----
    Bounds getBounds() const {
        return { position.x - size.x * 0.5f, position.y, size.x, size.y };
    }
    bool  isAlive()        const { return alive && health > 0.0f; }
    bool  isPlayer()       const { return faction == Faction::Player; }
    bool  isEnemy()        const { return faction == Faction::Enemy; }
    float healthFraction() const { return maxHealth > 0.0f ? clampf(health / maxHealth, 0.0f, 1.0f) : 0.0f; }
};

// Small helper so Member 1's CollisionSystem / HUD can print a unit name.
inline const char* unitTypeName(UnitType t) {
    switch (t) {
        case UnitType::MeleePlayer:   return "Melee (Kai)";
        case UnitType::RangedPlayer:  return "Ranged (Zane)";
        case UnitType::DefensePlayer: return "Defense (Cole)";
        case UnitType::EasyEnemy:     return "Easy (Venomari)";
        case UnitType::MediumEnemy:   return "Medium (Hypnobrai)";
        case UnitType::HardEnemy:     return "Hard (Constrictai)";
        default:                      return "None";
    }
}
