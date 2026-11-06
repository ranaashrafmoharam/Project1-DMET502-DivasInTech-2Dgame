#pragma once
// ====================================================================
// FILE:     src/entities/Enemy.h
// OWNER:    Member 2
// TASKS:    M2-02
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   The three enemy types (Easy upper-ranged, Medium lower-ranged,
//   Hard melee) with their stats and behaviour.
//
// RESPONSIBILITIES:
//   - Health: Easy 5, Medium 5, Hard 7                    (US-35/36/37)
//   - Damage 1 per attack, Hard deals 2                    (US-35/36/37)
//   - Advance left automatically, Hard runs faster         (US-37)
//   - Easy fires HIGH (duck), Medium fires LOW (jump),
//     Hard attacks at close range                          (US-35/36/37)
//   - takeDamage(), isDead(), drops a collectible on death (US-45/46)
//
// HOW MEMBER 1 DRIVES IT (CollisionSystem):
//       if (enemy.claimAttack())                       // once per swing
//           if (enemy.isRanged()) -> spawn a Projectile at
//                  enemy.muzzle(), tagged enemy.attackHeight()
//           else                   -> player.takeDamage(enemy.attackDamage())
//   and, when removing a dead enemy:
//       if (enemy.consumeOrbDrop()) -> spawnCollectible(...)
//
// INTERACTS WITH: Entity, CharacterModels, Animation, CollisionSystem,
//                 WaveManager, Projectile, Collectible
// ====================================================================

#include "Entity.h"
#include "HierarchicalModel.h"
#include "Projectile.h"      // AttackHeight
#include "CharacterModels.h"

class Enemy : public Entity {
public:
    Enemy();

    // type must be EasyEnemy / MediumEnemy / HardEnemy.
    void init(UnitType type, int lane, float spawnX = C::ENEMY_SPAWN_X);

    // ---- Entity interface -------------------------------------------
    void update(float dt) override;
    void draw() override;

    // ---- combat -----------------------------------------------------
    void takeDamage(float amount);
    void takeDamage(int amount) override { takeDamage(static_cast<float>(amount)); }
    bool isDead() const { return !isAlive(); }

    // True exactly once per swing, on the frame the attack connects.
    bool  claimAttack();
    bool  isAttacking() const { return attacking; }
    bool  isRanged()    const;

    // HIGH (comes from above, dodge by ducking) or LOW (from below,
    // dodge by jumping).
    AttackHeight attackHeight() const;
    float attackDamage() const;
    float attackRange()  const;
    float moveSpeedValue() const { return moveSpeed; }

    // Spawn socket (world space) for this enemy's shots.  A HIGH shot is
    // placed at head height, a LOW shot skims the ground, so the two
    // ranged enemies look and play differently.                   (US-35/36)
    Vec2  muzzle() const;
    float facing() const { return -1.0f; }    // enemies always face left

    // ---- collectible drop (US-46) ------------------------------------
    // Returns true once, immediately after the enemy is defeated.
    bool consumeOrbDrop();
    int  orbValue() const { return C::ORB_VALUE; }
    Bounds getBounds() const override;

    // Movement can be halted by the CollisionSystem during a fight.
    void setBlocked(bool b) { blocked = b; }
    bool isBlocked() const { return blocked; }

    HierarchicalModel model;

private:
    UnitType unitType = UnitType::None;
    void startAttack();
    void updatePose();

    static constexpr float HIT_FLINCH_TIME = 0.25f;

    // stats copied from Constants at init time
    float moveSpeed      = C::ENEMY_SPEED;
    float attackCooldown = C::ENEMY_ATTACK_COOLDOWN;

    // timers
    float animTime   = 0.0f;
    float cooldown   = 0.0f;
    float swingTimer = 0.0f;
    float hitTimer   = 0.0f;

    // state
    bool attacking  = false;
    bool attackUsed = false;
    bool blocked    = false;
    bool orbPending = false;    // set on death, cleared by consumeOrbDrop()

    float baseHeight = C::CHARACTER_HEIGHT;
};
