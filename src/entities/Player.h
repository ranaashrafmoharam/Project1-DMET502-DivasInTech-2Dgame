#pragma once
// ====================================================================
// FILE:     src/entities/Player.h
// OWNER:    Member 2
// TASKS:    M2-02, M2-12
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   The three player types (Melee, Ranged, Defense) with their stats
//   and behaviour.
//
// RESPONSIBILITIES:
//   - Health: Melee 5, Ranged 5, Defense 7 hits           (US-25/26/27)
//   - Damage 1 per attack (Defense deals none)            (US-25/26/27)
//   - Advance right automatically (Defense stays still)   (US-25/26/27)
//   - Attack cooldown and attack animation trigger        (US-25/26)
//   - Jump and duck state (Defense ignores both)          (US-29/30/31)
//   - takeDamage(), isDead(), shielded-by-Defense flag    (US-32/41/43)
//
// HOW MEMBER 1 DRIVES IT (CollisionSystem):
//   update(dt) is called every frame.  For combat, when the player is
//   in range of an enemy:
//       if (player.claimAttack())                 // true once per swing
//           if (player.isRanged()) -> spawn a Projectile at
//                  player.muzzle() with player.attackDamage()
//           else                   -> enemy.takeDamage(player.attackDamage())
//   Every frame the system also sets setBlocked(...) during a fight and
//   setShielded(...) when a Defense player stands between this player
//   and the enemy in the same lane (US-32).  Health is read with
//   healthFraction() by Member 4's HealthBar (US-44).
//
// INTERACTS WITH: Entity, CharacterModels, Animation, CollisionSystem,
//                 Projectile, HealthBar
// ====================================================================

#include "Entity.h"
#include "HierarchicalModel.h"
#include "Projectile.h"
#include "CharacterModels.h"

class Player : public Entity {
public:
    Player();

    // type must be MeleePlayer / RangedPlayer / DefensePlayer.
    void init(UnitType type, int lane, float spawnX = C::PLAYER_SPAWN_X);

    // ---- Entity interface -------------------------------------------
    void update(float dt) override;
    void draw() override;

    // ---- combat -----------------------------------------------------
    void takeDamage(float amount);
    void takeDamage(int amount) override { takeDamage(static_cast<float>(amount)); }
    bool isDead() const { return !isAlive(); }

    // True exactly once per swing, on the frame the weapon is in range.
    bool claimAttack();
    bool  isAttacking() const { return attacking; }
    bool  isRanged()    const { return unitType == UnitType::RangedPlayer; }
    float attackDamage() const;
    float attackRange()  const;
    AttackHeight attackHeight() const { return AttackHeight::High; }

    // Weapon socket (world space) and travel direction for a projectile.
    Vec2  muzzle() const;
    float facing() const { return 1.0f; }   // players always face right

    // ---- commands ----------------------------------------------------
    void jump() override;                     // US-29 (ignored by Defense, US-31)
    void duck() override;                     // US-30 (ignored by Defense, US-31)
    bool isJumping()   const { return jumping; }
    bool isDucking()   const { return ducking; }
    float jumpHeight() const { return jumpLift; }

    // ---- protection / movement flags set by the CollisionSystem -------
    void setShielded(bool s) { shielded = s; }   // US-32
    bool isShielded()  const { return shielded; }
    void setBlocked(bool b)  { blocked  = b; }   // stops advancing in a fight
    bool isBlocked()   const { return blocked; }
    bool isStationary() const { return unitType == UnitType::DefensePlayer; }
    Bounds getBounds() const override;

    HierarchicalModel model;

private:
    UnitType unitType = UnitType::None;
    void  startSwing();
    void  updatePose();
    float hitFlinchFraction() const { return hitTimer > 0.0f ? hitTimer / HIT_FLINCH_TIME : 0.0f; }

    static constexpr float HIT_FLINCH_TIME = 0.25f;
    static constexpr float ACTION_LOCKOUT_TIME = 0.25f;

    // stats copied from Constants at init time
    float moveSpeed      = C::PLAYER_SPEED;
    float attackCooldown = C::PLAYER_ATTACK_COOLDOWN;

    // timers
    float animTime    = 0.0f;   // free-running clock for walk / idle
    float cooldown    = 0.0f;   // seconds until the next swing may start
    float swingTimer  = 0.0f;   // counts up during a swing
    float hitTimer    = 0.0f;   // hit flinch fade
    float jumpTimer   = 0.0f;
    float duckTimer   = 0.0f;
    float jumpLockout = 0.0f;
    float duckLockout = 0.0f;

    // state
    bool  attacking  = false;
    bool  attackUsed = false;   // true once this swing has dealt its hit
    bool  jumping    = false;
    bool  ducking    = false;
    bool  blocked    = false;

    float jumpLift   = 0.0f;    // current height of the jump arc
    float duckAmount = 0.0f;    // 0 standing .. 1 fully crouched
    float baseHeight = C::CHARACTER_HEIGHT;
};
