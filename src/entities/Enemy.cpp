#include "Enemy.h"
#include "CharacterModels.h"
#include "Animation.h"
#include "../core/Renderer.h"
#include "../audio/AudioManager.h"
// ====================================================================
// FILE:     src/entities/Enemy.cpp
// OWNER:    Member 2
// TASKS:    M2-02
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   The three enemy types (Easy upper-ranged, Medium lower-ranged,
//   Hard melee) with their stats and behaviour.
//
// INTERACTS WITH: Entity, CharacterModels, Animation, CollisionSystem,
//                 WaveManager, Projectile, Collectible
// ====================================================================

#include <cmath>

namespace {
EntityType entityTypeFor(UnitType type) {
    switch (type) {
        case UnitType::EasyEnemy: return EntityType::ENEMY_EASY;
        case UnitType::MediumEnemy: return EntityType::ENEMY_MEDIUM;
        case UnitType::HardEnemy: return EntityType::ENEMY_HARD;
        default: return EntityType::ENEMY_EASY;
    }
}
}

Enemy::Enemy()
    : Entity(EntityType::ENEMY_EASY, 0, C::ENEMY_SPAWN_X,
             Constants::getLaneCenterY(0), static_cast<int>(C::ENEMY_EASY_HEALTH), C::ENEMY_SPEED) {}

// --------------------------------------------------------------------
// Build the unit: pick the model, then copy the stats for its type.
// --------------------------------------------------------------------
void Enemy::init(UnitType t, int lane_, float spawnX) {
    unitType = t;
    type     = entityTypeFor(t);
    laneIndex = lane_;
    alive    = true;
    isFighting = false;
    position = glm::vec2(spawnX, Constants::getLaneCenterY(lane_));

    model      = CharacterModels::buildForType(t);
    const Vec2 modelSize = model.size();
    size       = glm::vec2(modelSize.x, modelSize.y);
    baseHeight = modelSize.y;

    switch (t) {
        case UnitType::EasyEnemy:                          // US-35 upper ranged
            maxHealth      = C::ENEMY_EASY_HEALTH;         // 5 lives
            moveSpeed      = C::ENEMY_SPEED;               // normal speed
            attackCooldown = C::ENEMY_ATTACK_COOLDOWN;
            break;

        case UnitType::MediumEnemy:                        // US-36 lower ranged
            maxHealth      = C::ENEMY_MEDIUM_HEALTH;       // 5 lives
            moveSpeed      = C::ENEMY_SPEED;               // normal speed
            attackCooldown = C::ENEMY_ATTACK_COOLDOWN;
            break;

        case UnitType::HardEnemy:                          // US-37 melee brute
        default:
            maxHealth      = C::ENEMY_HARD_HEALTH;         // 7 lives
            moveSpeed      = C::HARD_ENEMY_SPEED;          // runs faster
            attackCooldown = C::HARD_ENEMY_COOLDOWN;
            break;
    }

    health = maxHealth;
    cooldown = 0.0f;
    swingTimer = 0.0f;
    hitTimer = 0.0f;
    animTime = 0.0f;
    attacking = false;
    attackUsed = false;
    blocked = false;
    orbPending = false;
    speed = moveSpeed;
    Entity::attackCooldown = attackCooldown;
    Entity::attackDamage = static_cast<int>(Enemy::attackDamage());

    // Lay the model out straight away so muzzle() / the hitbox are valid.
    updatePose();
}

// --------------------------------------------------------------------
// Combat queries
// --------------------------------------------------------------------
bool Enemy::isRanged() const {
    return unitType == UnitType::EasyEnemy || unitType == UnitType::MediumEnemy;
}

AttackHeight Enemy::attackHeight() const {
    // Easy (Venomari) spits from above  -> the player must duck  (US-35)
    // Medium (Hypnobrai) sweeps low     -> the player must jump  (US-36)
    // Hard is melee, so the tag is unused.
    return (unitType == UnitType::MediumEnemy) ? AttackHeight::Low : AttackHeight::High;
}

float Enemy::attackDamage() const {
    switch (unitType) {
        case UnitType::EasyEnemy:   return C::ENEMY_EASY_DAMAGE;     // 1
        case UnitType::MediumEnemy: return C::ENEMY_MEDIUM_DAMAGE;   // 1
        case UnitType::HardEnemy:   return C::ENEMY_HARD_DAMAGE;     // 2
        default:                    return 0.0f;
    }
}

float Enemy::attackRange() const {
    return isRanged() ? C::ENEMY_RANGED_RANGE : C::ENEMY_MELEE_RANGE;
}

bool Enemy::claimAttack() {
    // Match the hit to the middle of the weapon's strike animation.
    if (!attacking || attackUsed ||
        swingTimer < C::ATTACK_ANIM_TIME * Animation::ATTACK_HIT_PHASE) return false;
    attackUsed = true;
    return true;
}

Vec2 Enemy::muzzle() const {
    // Horizontal position comes from the model's own socket (mouth / staff
    // / fist); vertical position encodes HIGH vs LOW so the shot visibly
    // arrives from above or below.
    float socketX = position.x;
    if      (model.hasPart("snout"))    socketX = model.partWorldPosition("snout").x;
    else if (model.hasPart("staffTip")) socketX = model.partWorldPosition("staffTip").x;
    else if (model.hasPart("fistR"))    socketX = model.partWorldPosition("fistR").x;

    float feetY = C::laneCenterY(laneIndex);
    float shotY = (attackHeight() == AttackHeight::High)
                    ? feetY + baseHeight * 0.72f      // head height: duck (US-35)
                    : feetY + 6.0f;                   // ankle height: jump (US-36)

    return { socketX, shotY };
}

// --------------------------------------------------------------------
// The CollisionSystem only counts the hit while the enemy is in range.
// --------------------------------------------------------------------
void Enemy::startAttack() {
    attacking  = true;
    attackUsed = false;
    swingTimer = 0.0f;
    cooldown   = attackCooldown;

    // Attack sounds (US-56): each tribe sounds different.
    switch (unitType) {
        case UnitType::EasyEnemy:   getAudio().playSound(SoundId::VenomSpit);   break;
        case UnitType::MediumEnemy: getAudio().playSound(SoundId::SonicBlast);  break;
        case UnitType::HardEnemy:   getAudio().playSound(SoundId::MeleeCrush);  break;
        default:                    getAudio().playSound(SoundId::SnakeHiss);   break;
    }
}

// --------------------------------------------------------------------
// Damage taken (US-41 / US-43)
// --------------------------------------------------------------------
void Enemy::takeDamage(float amount) {
    if (!isAlive() || amount <= 0.0f) return;

    health  -= amount;
    hitTimer = HIT_FLINCH_TIME;
    getAudio().playSound(SoundId::Hit);

    if (health <= 0.0f) {
        health     = 0.0f;
        alive      = false;        // US-45: removed immediately by the Game loop
        orbPending = true;         // US-46: the Game spawns a collectible for this
        getAudio().playSound(SoundId::Death);
    }
}

bool Enemy::consumeOrbDrop() {
    if (!orbPending) return false;
    orbPending = false;
    return true;
}

Bounds Enemy::getBounds() const {
    return { position.x - size.x * 0.5f, position.y - size.y * 0.5f,
             size.x, size.y };
}

// --------------------------------------------------------------------
// Per-frame update
// --------------------------------------------------------------------
void Enemy::update(float dt) {
    if (!isAlive()) return;

    blocked = isFighting;
    animTime += dt;
    if (cooldown > 0.0f) cooldown -= dt;
    if (hitTimer > 0.0f) hitTimer -= dt;

    // ---- attack -----------------------------------------------------
    // "Enemies start attacking as soon as they appear, even if no players
    // are present", so a new swing simply loops.
    if (!attacking && cooldown <= 0.0f) startAttack();

    if (attacking) {
        swingTimer += dt;
        if (swingTimer >= C::ATTACK_ANIM_TIME) attacking = false;
    }

    // ---- movement (left, toward the home base) ----------------------
    if (!blocked && position.x > C::HOME_BASE_X) position.x -= moveSpeed * dt;

    position.y = Constants::getLaneCenterY(laneIndex);

    updatePose();
}

// --------------------------------------------------------------------
// Poses and the model root transform.
// --------------------------------------------------------------------
void Enemy::updatePose() {
    model.resetPoses();

    const float attackPhase = attacking ? clampf(swingTimer / C::ATTACK_ANIM_TIME, 0.0f, 1.0f) : 0.0f;

    // Easy / Medium slither (walk cycle drives their tail + coil);
    // Hard runs with a wider stride (US-37).
    if (blocked) Animation::updateIdle(model, animTime);
    else if (unitType == UnitType::HardEnemy) Animation::updateRun(model, animTime);
    else                                  Animation::updateWalk(model, animTime);

    if (attacking) Animation::updateAttack(model, animTime, attackPhase, isRanged());
    if (hitTimer > 0.0f) Animation::updateHit(model, hitTimer / HIT_FLINCH_TIME);

    // Enemies face left, so the whole model is mirrored in x.
    model.setRootTransform(Transform2D(
        { position.x, C::WORLD_HEIGHT - (position.y + baseHeight * 0.5f) },
        0.0f, { -1.0f, 1.0f }));
}

// --------------------------------------------------------------------
// Draw: same red damage tint convention as the players (US-43).
// --------------------------------------------------------------------
void Enemy::draw() {
    if (!isAlive()) return;

    Renderer& r = getRenderer();
    r.setTint(Vec4{ 0.86f, 0.10f, 0.10f, 1.0f });   // the red damage tint (US-43)
    r.setTintAmount(1.0f - healthFraction());       // stronger as health drops
    model.draw(r);
    r.setTintAmount(0.0f);
    r.setTint(Vec4{ 1, 1, 1, 1 });
}
