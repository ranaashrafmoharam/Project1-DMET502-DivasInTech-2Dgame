#include "Projectile.h"
#include "../core/Renderer.h"
#include "../audio/AudioManager.h"
// ====================================================================
// FILE:     src/entities/Projectile.cpp
// OWNER:    Member 2
// TASKS:    M2-11
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   A shot fired by the Ranged Player or a ranged enemy.  It travels
//   along its lane, animates in flight, and vanishes when it hits.
//
// INTERACTS WITH: Entity, Renderer, CollisionSystem, PropModels
// ====================================================================

#include <cmath>

namespace {

    // ---- the three shot looks (US-54: player and enemy weapons differ) ----
    const Vec4 STEEL     { 0.80f, 0.85f, 0.92f, 1.0f };
    const Vec4 ICE       { 0.45f, 0.78f, 0.98f, 1.0f };
    const Vec4 ICE_LIGHT { 0.80f, 0.94f, 1.00f, 1.0f };
    const Vec4 VENOM     { 0.45f, 0.72f, 0.20f, 1.0f };
    const Vec4 VENOM_HI  { 0.75f, 0.92f, 0.42f, 1.0f };
    const Vec4 SONIC     { 0.22f, 0.62f, 0.66f, 1.0f };
    const Vec4 SONIC_HI  { 0.65f, 0.92f, 0.92f, 1.0f };

    // Zane's thrown shuriken: a spinning four-bladed star.
    void drawShuriken(Renderer& r, float spin, float w) {
        float half = w * 0.5f;
        r.drawCircle({ 0.0f, 0.0f }, half * 0.55f, STEEL);
        for (int i = 0; i < 4; ++i) {
            float a = spin + (float)i * 1.5707963f;
            Vec2 dir{ std::cos(a), std::sin(a) };
            Vec2 side{ -dir.y, dir.x };
            Vec2 tip  = dir * (half * 1.10f);
            Vec2 base1 = side * (half * 0.30f);
            Vec2 base2 = base1 * -1.0f;
            r.drawTriangle(tip, base1, base2, i % 2 == 0 ? STEEL : ICE);
        }
        r.drawCircle({ 0.0f, 0.0f }, half * 0.22f, ICE_LIGHT);
    }

    // Venomari spits a venom glob that wobbles as it flies.
    void drawVenomGlob(Renderer& r, float age, float w) {
        float half = w * 0.5f;
        float wobble = std::sin(age * 18.0f) * 1.5f;
        r.drawCircle({ 0.0f, wobble }, half * 1.05f, VENOM);
        r.drawCircle({ -half * 0.2f, half * 0.25f + wobble }, half * 0.42f, VENOM_HI);
        r.drawTriangle({ -half, -half * 0.6f }, { -half * 2.0f, 0.0f }, { -half, half * 0.6f }, VENOM);
    }

    // Hypnobrai fires a pulsing sonic ring low along the ground.
    void drawSonicBlast(Renderer& r, float age, float w) {
        float half = w * 0.5f;
        float pulse = 1.0f + std::sin(age * 16.0f) * 0.18f;
        r.drawCircle({ 0.0f, 0.0f }, half * 1.15f * pulse, SONIC);
        r.drawCircle({ 0.0f, 0.0f }, half * 0.62f * pulse, SONIC_HI);
        r.drawCircle({ 0.0f, 0.0f }, half * 0.28f, Vec4{ 1, 1, 1, 1 });
    }

} // anonymous namespace

void Projectile::spawn(const Vec2& spawnPos, int lane_, Faction faction_,
                       UnitType ownerType_, AttackHeight height_,
                       float direction_, float damage_) {
    position    = spawnPos;
    lane        = lane_;
    faction     = faction_;
    ownerType   = ownerType_;
    height      = height_;
    direction   = (direction_ < 0.0f) ? -1.0f : 1.0f;
    damage      = damage_;
    speed       = C::PROJECTILE_SPEED;

    spin        = 0.0f;
    age         = 0.0f;

    type        = UnitType::None;   // a shot is not a unit
    health      = 1.0f;
    maxHealth   = 1.0f;
    alive       = true;
    size        = { C::PROJECTILE_WIDTH, C::PROJECTILE_WIDTH };
}

void Projectile::update(float dt) {
    if (!alive) return;

    age  += dt;
    spin += dt * 14.0f;                       // in-flight animation
    position.x += direction * speed * dt;     // travel along the lane

    // Leave the playfield -> the shot is spent.
    if (position.x < -60.0f || position.x > C::WORLD_WIDTH + 60.0f)
        alive = false;
}

bool Projectile::canHit(const Entity& target) const {
    return isAlive() && target.isAlive() &&
           faction != Faction::None && target.faction != Faction::None &&
           faction != target.faction;
}

void Projectile::draw() {
    if (!alive) return;

    Renderer& r = getRenderer();

    // Draw at the centre of the shot's box.
    Vec2 centre{ position.x, position.y + size.y * 0.5f };
    r.pushTransform(Transform2D(centre, 0.0f, { 1.0f, 1.0f }));

    switch (ownerType) {
        case UnitType::RangedPlayer:  drawShuriken(r, spin * direction, size.x); break;
        case UnitType::EasyEnemy:     drawVenomGlob(r, age, size.x);            break;
        case UnitType::MediumEnemy:   drawSonicBlast(r, age, size.x);           break;
        default:                      r.drawQuad({ 0, 0 }, size, STEEL);        break;
    }

    r.popTransform();
}

void Projectile::onHit() {
    // The shot disappears the instant it connects (US-42) and the
    // matching impact sound plays (US-56).
    alive = false;

    switch (ownerType) {
        case UnitType::RangedPlayer: getAudio().playSound(SoundId::IceBlast);    break;
        case UnitType::EasyEnemy:    getAudio().playSound(SoundId::VenomSpit);   break;
        case UnitType::MediumEnemy:  getAudio().playSound(SoundId::SonicBlast);  break;
        default: break;
    }
}
