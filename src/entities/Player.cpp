#include "Player.h"
#include "CharacterModels.h"
#include "Animation.h"
#include "../core/Renderer.h"
#include "../audio/AudioManager.h"
// ====================================================================
// FILE:     src/entities/Player.cpp
// OWNER:    Member 2
// TASKS:    M2-02, M2-12
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   The three player types (Melee / Ranged / Defense) with their stats
//   and behaviour.
//
// INTERACTS WITH: Entity, CharacterModels, Animation, CollisionSystem,
//                 Projectile, HealthBar
// ====================================================================

#include <cmath>

namespace {
EntityType entityTypeFor(UnitType type) {
    switch (type) {
        case UnitType::MeleePlayer: return EntityType::PLAYER_MELEE;
        case UnitType::RangedPlayer: return EntityType::PLAYER_RANGED;
        case UnitType::DefensePlayer: return EntityType::PLAYER_DEFENSE;
        default: return EntityType::PLAYER_MELEE;
    }
}
}

Player::Player()
    : Entity(EntityType::PLAYER_MELEE, 0, C::PLAYER_SPAWN_X,
             Constants::getLaneCenterY(0), static_cast<int>(C::MELEE_HEALTH), C::PLAYER_SPEED) {}

// --------------------------------------------------------------------
// Build the unit: pick the model, then copy the stats for its type.
// --------------------------------------------------------------------
void Player::init(UnitType t, int lane_, float spawnX) {
    unitType = t;
    type     = entityTypeFor(t);
    laneIndex = lane_;
    alive    = true;
    isFighting = false;
    shielded = false;
    Entity::isJumping = false;
    Entity::isDucking = false;
    position = glm::vec2(spawnX, Constants::getLaneCenterY(lane_));

    model      = CharacterModels::buildForType(t);
    const Vec2 modelSize = model.size();
    size       = glm::vec2(modelSize.x, modelSize.y);
    baseHeight = modelSize.y;

    switch (t) {
        case UnitType::MeleePlayer:                       // US-25
            maxHealth      = C::MELEE_HEALTH;             // 5 lives
            moveSpeed      = C::PLAYER_SPEED;             // advances right
            attackCooldown = C::PLAYER_ATTACK_COOLDOWN;
            break;

        case UnitType::RangedPlayer:                      // US-26
            maxHealth      = C::RANGED_HEALTH;            // 5 lives
            moveSpeed      = C::PLAYER_SPEED;             // advances right
            attackCooldown = C::PLAYER_ATTACK_COOLDOWN;
            break;

        case UnitType::DefensePlayer:                     // US-27
        default:
            maxHealth      = C::DEFENSE_HEALTH;           // absorbs 7 hits
            moveSpeed      = C::DEFENSE_SPEED;            // 0: never moves
            attackCooldown = 0.0f;                        // does not attack
            break;
    }

    health = maxHealth;
    cooldown = 0.0f;
    swingTimer = 0.0f;
    hitTimer = 0.0f;
    jumpTimer = 0.0f;
    duckTimer = 0.0f;
    jumpLockout = 0.0f;
    duckLockout = 0.0f;
    animTime = 0.0f;
    attacking = false;
    attackUsed = false;
    jumping = false;
    ducking = false;
    blocked = false;
    jumpLift = 0.0f;
    duckAmount = 0.0f;
    speed = moveSpeed;
    Entity::attackCooldown = attackCooldown;
    Entity::attackDamage = static_cast<int>(Player::attackDamage());

    // Lay the model out in its resting pose straight away so muzzle()
    // and the hitbox are valid on the very first frame.
    updatePose();
}

// --------------------------------------------------------------------
// The CollisionSystem / debug text can ask for one hit per swing.
// --------------------------------------------------------------------
bool Player::claimAttack() {
    // Match the hit to the middle of the weapon's strike animation.
    if (!attacking || attackUsed ||
        swingTimer < C::ATTACK_ANIM_TIME * Animation::ATTACK_HIT_PHASE) return false;
    attackUsed = true;
    return true;
}

float Player::attackDamage() const {
    switch (unitType) {
        case UnitType::MeleePlayer:   return C::PLAYER_DAMAGE;   // 1
        case UnitType::RangedPlayer:  return C::PLAYER_DAMAGE;   // 1, carried by the shot
        default:                      return C::DEFENSE_DAMAGE;  // 0, Defense never attacks
    }
}

float Player::attackRange() const {
    switch (unitType) {
        case UnitType::MeleePlayer:   return C::MELEE_RANGE;
        case UnitType::RangedPlayer:  return C::RANGED_RANGE;
        default:                      return 0.0f;
    }
}

Vec2 Player::muzzle() const {
    if (model.hasPart("weapon")) return model.partWorldPosition("weapon");
    if (model.hasPart("handR"))  return model.partWorldPosition("handR");
    // Fallback: just in front of the chest.
    return { position.x + facing() * 12.0f,
             C::WORLD_HEIGHT - position.y + baseHeight * 0.1f };
}

Bounds Player::getBounds() const {
    const float h = size.y;
    const float centerY = position.y - jumpLift;
    return { position.x - size.x * 0.5f, centerY - h * 0.5f, size.x, h };
}

// --------------------------------------------------------------------
// Commands
// --------------------------------------------------------------------
void Player::jump() {
    // US-31: jump and duck do not affect the Defense player.
    if (isStationary() || jumping || jumpLockout > 0.0f) return;
    jumping   = true;
    jumpTimer = 0.0f;
    jumpLockout = C::JUMP_DURATION + ACTION_LOCKOUT_TIME;
}

void Player::duck() {
    if (isStationary() || ducking || duckLockout > 0.0f) return;
    ducking   = true;
    duckTimer = 0.0f;
    duckLockout = C::DUCK_DURATION + ACTION_LOCKOUT_TIME;
}

void Player::startSwing() {
    attacking  = true;
    attackUsed = false;
    swingTimer = 0.0f;
    cooldown   = attackCooldown;

    // Attack sounds (US-56): the two players have different weapons.
    if (unitType == UnitType::MeleePlayer)       getAudio().playSound(SoundId::SwordSwing);
    else if (unitType == UnitType::RangedPlayer) getAudio().playSound(SoundId::ShurikenThrow);
}

// --------------------------------------------------------------------
// Damage taken (US-41 / US-43)
// --------------------------------------------------------------------
void Player::takeDamage(float amount) {
    if (!isAlive() || amount <= 0.0f) return;

    // US-32: a Defense player in front protects anyone behind it, so a
    // shielded player simply shrugs the hit off.
    if (Entity::shielded) return;

    health  -= amount;
    hitTimer = HIT_FLINCH_TIME;
    getAudio().playSound(SoundId::Hit);

    if (health <= 0.0f) {
        health = 0.0f;
        alive  = false;                   // US-45: removed by the Game loop
        getAudio().playSound(SoundId::Death);
    }
}

// --------------------------------------------------------------------
// Per-frame update
// --------------------------------------------------------------------
void Player::update(float dt) {
    if (!isAlive()) return;

    animTime += dt;
    blocked = isFighting;
    if (cooldown > 0.0f) cooldown -= dt;
    if (hitTimer > 0.0f) hitTimer -= dt;
    if (jumpLockout > 0.0f) jumpLockout -= dt;
    if (duckLockout > 0.0f) duckLockout -= dt;

    // ---- attack -----------------------------------------------------
    // Players "start attacking as soon as they are placed, even if no
    // enemies are present", so a new swing simply loops.  Defense has a
    // zero cooldown but is skipped here because it never attacks.
    if (!isStationary() && !attacking && cooldown <= 0.0f) startSwing();

    if (attacking) {
        swingTimer += dt;
        if (swingTimer >= C::ATTACK_ANIM_TIME) attacking = false;
    }

    // ---- movement (US-25 / US-26 / US-27) ---------------------------
    const bool canAdvance = !isStationary() && !blocked && position.x < C::PLAYER_MAX_X;
    if (canAdvance) position.x += moveSpeed * dt;

    // ---- jump arc (US-29) -------------------------------------------
    if (jumping) {
        jumpTimer += dt;
        float p = jumpTimer / C::JUMP_DURATION;
        if (p >= 1.0f) { jumping = false; jumpLift = 0.0f; }
        else           { jumpLift = C::JUMP_HEIGHT * std::sin(3.14159265f * p); }
    }

    // ---- duck pose (US-30) ------------------------------------------
    if (ducking) {
        duckTimer += dt;
        float p = duckTimer / C::DUCK_DURATION;
        if (p >= 1.0f) {
            ducking = false;
            duckAmount = 0.0f;
        } else {
            // Rise quickly, hold the crouch, then rise back up.
            float ramp = std::min(p, 1.0f - p) / 0.18f;
            duckAmount = clampf(ramp, 0.0f, 1.0f);
        }
    }

    // ---- geometry ---------------------------------------------------
    position.y = Constants::getLaneCenterY(laneIndex);
    // A ducking unit is a shorter target (helps the CollisionSystem).
    size.y = baseHeight * lerpf(1.0f, C::DUCK_SCALE_Y, duckAmount);
    Entity::isJumping = jumping;
    Entity::isDucking = ducking;
    Entity::jumpTimer = jumping ? jumpTimer : 0.0f;
    Entity::duckTimer = ducking ? duckTimer : 0.0f;

    updatePose();
}

// --------------------------------------------------------------------
// Poses and the model root transform.
// --------------------------------------------------------------------
void Player::updatePose() {
    model.resetPoses();

    const float attackPhase = attacking ? clampf(swingTimer / C::ATTACK_ANIM_TIME, 0.0f, 1.0f) : 0.0f;

    if (jumping) {
        Animation::updateJump(model, jumpTimer / C::JUMP_DURATION);   // US-29
    } else {
        if (duckAmount > 0.001f)            Animation::updateDuck(model, duckAmount);  // US-30
        else if (isStationary() || blocked) Animation::updateIdle(model, animTime);
        else                                Animation::updateWalk(model, animTime);    // US-25 / US-26

        if (attacking)                      Animation::updateAttack(model, animTime, attackPhase, isRanged());
    }

    if (hitTimer > 0.0f) Animation::updateHit(model, hitFlinchFraction());             // US-43

    // Players always face right, so no mirroring is needed.
    model.setRootTransform(Transform2D(
        { position.x, C::WORLD_HEIGHT - (position.y + baseHeight * 0.5f - jumpLift) },
        0.0f, { 1.0f, 1.0f }));
}

// --------------------------------------------------------------------
// Draw: the red damage tint grows as health drops (US-43).  The visible
// health bar itself is Member 4's HealthBar, which reads healthFraction().
// --------------------------------------------------------------------
void Player::draw() {
    if (!isAlive()) return;

    Renderer& r = getRenderer();
    r.setTint(Vec4{ 0.86f, 0.10f, 0.10f, 1.0f });   // the red damage tint (US-43)
    r.setTintAmount(1.0f - healthFraction());       // stronger as health drops
    model.draw(r);
    r.setTintAmount(0.0f);
    r.setTint(Vec4{ 1, 1, 1, 1 });
}
