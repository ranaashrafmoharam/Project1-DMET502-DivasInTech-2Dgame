#pragma once
// ====================================================================
// FILE:     src/entities/Projectile.h
// OWNER:    Member 2
// TASKS:    M2-11
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   A shot fired by the Ranged Player (US-26) or a ranged enemy
//   (US-35 upper / US-36 lower).
//
// RESPONSIBILITIES:
//   - Spawn at the weapon/snout socket, travel along one lane
//   - Tagged HIGH (must be ducked) or LOW (must be jumped)
//   - Animate in flight (spin / wobble) and disappear on collision
//   - Expose a hitbox for the CollisionSystem
//
// It derives from Entity so Member 1's systems can keep one list of
// Entity* and treat projectiles like everything else.  As with every
// Entity, `position` is the BOTTOM-CENTRE of the shot's box.
//
// INTERACTS WITH: Entity, Renderer, CollisionSystem, PropModels
// ====================================================================

#include "Entity.h"
#include "../core/Math.h"

// HIGH shots fly at head height and are dodged by ducking (US-30).
// LOW shots skim the ground and are dodged by jumping (US-29).
enum class AttackHeight { High, Low };

class Projectile : public Entity {
public:
    Projectile();

    // spawnPos  : bottom-centre of the shot (use Player/Enemy muzzle()).
    // direction : +1 travels right (player shots), -1 travels left (enemy).
    void spawn(const Vec2& spawnPos, int lane, Faction faction, UnitType ownerType,
               AttackHeight height, float direction, float damage);

    // ---- Entity interface -------------------------------------------
    void update(float dt) override;
    void draw() override;

    // ---- queries the CollisionSystem uses ---------------------------
    bool isHigh() const { return height == AttackHeight::High; }
    bool isLow()  const { return height == AttackHeight::Low; }
    AttackHeight attackHeight() const { return height; }
    UnitType getOwnerType() const { return ownerType; }
    float getDamage() const { return damage; }
    float getSpeed()  const { return speed; }
    float getSpin() const { return spin; }
    // CollisionSystem should check this before applying projectile damage.
    bool canHit(const Entity& target) const;

    // ---- lifecycle ---------------------------------------------------
    // Called by the CollisionSystem the moment the shot connects: the
    // shot is gone immediately (US-42) and the impact sound plays (US-56).
    void onHit();

private:
    Faction      projectileFaction = Faction::None;
    AttackHeight height      = AttackHeight::High;
    UnitType     ownerType   = UnitType::None;
    float        damage      = 1.0f;
    float        direction   = 1.0f;      // +1 right, -1 left
    float        speed       = C::PROJECTILE_SPEED;
    float        spin        = 0.0f;      // in-flight rotation
    float        age         = 0.0f;      // seconds alive (for wobble)
};
