#include "PropModels.h"
// ====================================================================
// FILE:     src/scene/PropModels.cpp
// OWNER:    Member 3
// TASKS:    M3-09, M3-10, US-48, US-54
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Weapon, projectile and collectible visuals in Ninjago theme:
//   - Melee: Sword of Fire (flaming Katana with ruby pommel)
//   - Ranged: Zane's Shurikens of Ice & Frost Bow
//   - Projectile 0: Zane's spinning Ice Shuriken
//   - Projectile 1: Venomari toxic acid venom spit (from above)
//   - Projectile 2: Hypnobrai sonic hypnotic rings (from below)
//   - Collectible: Golden Weapon Shard with 3D spin & sparkles
// ====================================================================

#include "Scene.h"
#include <cmath>

void PropModels::drawMeleeWeapon(float x, float y, float angle)
{
    float dx = std::cos(angle);
    float dy = std::sin(angle);
    float nx = -dy;
    float ny =  dx;
    float t = M3Draw::getTime();

    // 1. Gold Pommel & Red Wrapped Handle (Tsuka)
    M3Draw::circle(x - dx * 14, y - dy * 14, 5, 0.95f, 0.78f, 0.22f); // Gold pommel
    M3Draw::line(x - dx * 12, y - dy * 12, x, y, 6, 0.78f, 0.16f, 0.12f); // Red wrap
    // Handle wrap diamond cord pattern
    M3Draw::quad(x - dx * 6 - 2, y - dy * 6 - 2, 4, 4, 0.95f, 0.85f, 0.30f);

    // 2. Ornate Golden Crossguard (Tsuba) with Dragon Ruby
    M3Draw::line(x + nx * 10, y + ny * 10, x - nx * 10, y - ny * 10, 5, 0.95f, 0.78f, 0.22f);
    M3Draw::circle(x, y, 3, 0.95f, 0.18f, 0.18f); // Center ruby gem

    // 3. Steel Blade (Katana)
    float bladeLen = 38.0f;
    float tipX = x + dx * bladeLen;
    float tipY = y + dy * bladeLen;
    M3Draw::line(x, y, tipX, tipY, 5, 0.88f, 0.90f, 0.94f);      // Blade core
    M3Draw::line(x, y, tipX, tipY, 2, 1.00f, 1.00f, 1.00f);      // Sharp edge highlight

    // 4. Dancing Pixel Fire Flames along the blade!
    for (int f = 1; f <= 5; ++f)
    {
        float pos = f * 0.18f;
        float fx = x + dx * bladeLen * pos;
        float fy = y + dy * bladeLen * pos;
        float flameSway = std::sin(t * 12.0f + f * 1.5f) * 6.0f;

        // Orange flame body
        M3Draw::quad(fx + nx * flameSway - 3, fy + ny * flameSway - 3, 6, 6, 1.00f, 0.45f, 0.10f, 0.85f);
        // Yellow flame core
        M3Draw::quad(fx + nx * (flameSway * 0.7f) - 2, fy + ny * (flameSway * 0.7f) - 2, 4, 4, 1.00f, 0.90f, 0.25f, 0.95f);
    }
}

void PropModels::drawRangedWeapon(float x, float y, float angle)
{
    float dx = std::cos(angle);
    float dy = std::sin(angle);
    float nx = -dy;
    float ny =  dx;
    float t = M3Draw::getTime();

    // Zane's Frost Recurve Bow (Crafted from frosted cyan ice and silver)
    // Grip
    M3Draw::quad(x - 3, y - 3, 6, 6, 0.85f, 0.88f, 0.92f);

    // Upper Bow Limb
    // Use the same angle axis as the sword so the bow is visibly parallel
    // to it, pointing diagonally toward the right-hand side of the HUD.
    float u1X = x + dx * 14 + nx * 6;
    float u1Y = y + dy * 14 + ny * 6;
    float u2X = x + dx * 24 - nx * 8;
    float u2Y = y + dy * 24 - ny * 8;
    M3Draw::line(x, y, u1X, u1Y, 4, 0.38f, 0.82f, 0.95f);
    M3Draw::line(u1X, u1Y, u2X, u2Y, 3, 0.65f, 0.92f, 1.00f);
    M3Draw::circle(u2X, u2Y, 3, 0.95f, 0.98f, 1.00f); // Ice crystal tip

    // Lower Bow Limb
    float l1X = x - dx * 14 + nx * 6;
    float l1Y = y - dy * 14 + ny * 6;
    float l2X = x - dx * 24 - nx * 8;
    float l2Y = y - dy * 24 - ny * 8;
    M3Draw::line(x, y, l1X, l1Y, 4, 0.38f, 0.82f, 0.95f);
    M3Draw::line(l1X, l1Y, l2X, l2Y, 3, 0.65f, 0.92f, 1.00f);
    M3Draw::circle(l2X, l2Y, 3, 0.95f, 0.98f, 1.00f);

    // Glowing Bowstring
    M3Draw::line(u2X, u2Y, l2X, l2Y, 1.5f, 0.80f, 0.95f, 1.00f, 0.85f);

    // Animated frost accents: the bow stays fixed, while icy highlights travel
    // along both limbs like the sword's moving elemental effect.
    for (int i = 1; i <= 4; ++i)
    {
        float p = i / 5.0f;
        float pulse = 0.5f + 0.5f * std::sin(t * 8.0f + i * 1.6f);
        float upperX = x + (u2X - x) * p;
        float upperY = y + (u2Y - y) * p;
        float lowerX = x + (l2X - x) * p;
        float lowerY = y + (l2Y - y) * p;
        float drift = std::sin(t * 8.0f + i * 1.6f) * 2.5f;

        M3Draw::circle(upperX + nx * drift, upperY + ny * drift,
                       2.2f + pulse * 1.2f, 0.55f, 0.90f, 1.00f,
                       0.55f + pulse * 0.4f, 8);
        M3Draw::circle(lowerX - nx * drift, lowerY - ny * drift,
                       2.2f + pulse * 1.2f, 0.55f, 0.90f, 1.00f,
                       0.55f + pulse * 0.4f, 8);
    }
}

void PropModels::drawProjectile(int type, float x, float y, float rotation)
{
    float t = M3Draw::getTime();

    if (type == 0)
    {
        // =====================================================================
        // TYPE 0: ZANE'S SHURIKEN OF ICE (Spinning Crystalline Ice Star)
        // =====================================================================
        float spin = rotation + t * 14.0f;
        float r = 12.0f;

        for (int arm = 0; arm < 4; ++arm)
        {
            float a = spin + arm * 1.570796f;
            float px = x + std::cos(a) * r;
            float py = y + std::sin(a) * r;
            float nx = -std::sin(a) * 4.0f;
            float ny =  std::cos(a) * 4.0f;

            // Ice blade arm
            M3Draw::triangle(x, y, px, py, x + nx, y + ny, 0.45f, 0.85f, 0.98f);
            M3Draw::triangle(x, y, px, py, x - nx, y - ny, 0.75f, 0.94f, 1.00f);
        }
        // Center crystal hub
        M3Draw::circle(x, y, 4.5f, 1.00f, 1.00f, 1.00f);
        // Frost glow
        M3Draw::circle(x, y, 14.0f, 0.45f, 0.85f, 1.00f, 0.25f);
    }
    else if (type == 1)
    {
        // =====================================================================
        // TYPE 1: VENOMARI ACID SPIT (High Ranged Enemy Projectile - Duck to Dodge!)
        // =====================================================================
        // Toxic bubbling green venom orb
        float pulse = 0.90f + 0.10f * std::sin(t * 16.0f);
        M3Draw::circle(x, y, 10.0f * pulse, 0.28f, 0.82f, 0.22f, 0.9f);
        M3Draw::circle(x - 2, y - 2, 6.0f * pulse, 0.55f, 0.95f, 0.35f, 1.0f); // Bright core
        M3Draw::circle(x - 4, y - 4, 3.0f, 0.85f, 1.00f, 0.65f, 1.0f);         // Specular highlight

        // Dripping acid droplets trailing behind
        float dx = std::cos(rotation);
        float dy = std::sin(rotation);
        for (int d = 1; d <= 3; ++d)
        {
            float trailX = x - dx * d * 10.0f + std::sin(t * 10.0f + d) * 3.0f;
            float trailY = y - dy * d * 10.0f + (d * 3.0f);
            float dropR = (4.0f - d) * 1.5f;
            M3Draw::circle(trailX, trailY, dropR, 0.32f, 0.88f, 0.26f, 0.75f);
        }
    }
    else
    {
        // =====================================================================
        // TYPE 2: HYPNOBRAI SONIC / HYPNO RINGS (Low Ranged Enemy - Jump to Dodge!)
        // =====================================================================
        // Pulsing purple concentric sonic rings
        for (int ring = 0; ring < 3; ++ring)
        {
            float ringPhase = std::fmod(t * 3.0f + ring * 0.33f, 1.0f);
            float ringRadius = 6.0f + ringPhase * 16.0f;
            float ringAlpha = (1.0f - ringPhase) * 0.85f;
            M3Draw::circle(x, y, ringRadius, 0.72f, 0.22f, 0.88f, ringAlpha, 16);
            M3Draw::circle(x, y, ringRadius - 2.5f, 0.18f, 0.10f, 0.25f, ringAlpha, 14); // Hollow ring
        }
        // Center hypnotic seed
        M3Draw::circle(x, y, 5.0f, 0.95f, 0.40f, 1.00f);
        M3Draw::circle(x, y, 2.5f, 1.00f, 0.85f, 1.00f);
    }
}

void PropModels::drawCollectible(float x, float y, float spin)
{
    float t = M3Draw::getTime();

    // Floating bobbing animation
    float bob = std::sin(spin * 3.0f) * 6.0f;
    float curY = y + bob;

    // 3D Rotating Golden Weapon Shard
    float cosVal = std::cos(spin * 2.5f);
    float shardW = 6.0f + std::abs(cosVal) * 14.0f;
    float shardH = 26.0f;

    // Golden aura glow around shard
    M3Draw::circle(x, curY, shardW + 12.0f, 0.98f, 0.82f, 0.25f, 0.28f, 16);

    // Faceted Crystal Shard
    bool frontFace = cosVal >= 0.0f;
    // Left facet
    M3Draw::triangle(x, curY - shardH, x - shardW, curY, x, curY + shardH,
                     frontFace ? 0.92f : 0.75f, frontFace ? 0.74f : 0.55f, 0.16f);
    // Right facet
    M3Draw::triangle(x, curY - shardH, x + shardW, curY, x, curY + shardH,
                     frontFace ? 1.00f : 0.85f, frontFace ? 0.90f : 0.68f, frontFace ? 0.35f : 0.20f);

    // Specular light glint running across facet
    float glintX = x + cosVal * shardW * 0.4f;
    M3Draw::line(glintX, curY - shardH * 0.6f, glintX, curY + shardH * 0.6f, 2, 1.0f, 1.0f, 0.90f, 0.85f);

    // Orbiting Golden Sparkle Pixels
    for (int s = 0; s < 4; ++s)
    {
        float sa = spin * 3.0f + s * 1.570796f;
        float sx = x + std::cos(sa) * (shardW + 10.0f);
        float sy = curY + std::sin(sa) * 14.0f;
        float sparklePulse = 0.5f + 0.5f * std::sin(t * 8.0f + s);
        M3Draw::quad(sx - 2, sy - 2, 4, 4, 1.0f, 0.95f, 0.55f, sparklePulse);
    }
}
