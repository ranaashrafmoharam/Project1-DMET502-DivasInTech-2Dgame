#pragma once
#include "M3Draw.h"
// ====================================================================
// FILE:     src/scene/CharacterSprites.h
// OWNER:    Member 3
// TASKS:    US-06, US-07, US-08, US-09, US-10, US-51, US-52, US-54
//
// PURPOSE:
//   Aseprite / Penusbmic style pixel art character rendering & animation:
//   - Clean transparent sprites sampled from uploaded character art
//   - 4-Keyframe Bouncy Running Cycles (Contact, Squash, Passing, Stretch)
//   - Punchy 4-Phase Attacking Animations (Windup, Lunge Smear, Impact Hold, Recovery)
//   - Realistic Serpentine S-Curve Slither for Villains
//   - Full particle FX: Fire slash arcs, Frost shurikens, Earth quakes, Acid spit
// ====================================================================

#include "CharacterGridData.h"
#include "Scene.h"
#include <cmath>
#include <cstdint>

namespace CharacterSprites
{

enum class Character
{
    KAI,
    ZANE,
    COLE,
    WU,
    VENOMARI,
    PYTHOR,
    GARMADON
};

enum class State
{
    IDLE,
    RUN,
    ATTACK,
    JUMP,
    DUCK,
    HIT
};

inline void unpackRGB(uint32_t c, float& r, float& g, float& b)
{
    r = static_cast<float>((c >> 16) & 0xFF) / 255.0f;
    g = static_cast<float>((c >> 8) & 0xFF) / 255.0f;
    b = static_cast<float>(c & 0xFF) / 255.0f;
}

// --------------------------------------------------------------------
// Core Matrix Renderer with Discrete Aseprite Keyframing Offsets
// --------------------------------------------------------------------
inline void renderSprite(const uint32_t grid[24][18],
                         float posX, float posY, float scale,
                         bool facingRight,
                         float headY, float torsoY, float torsoLean,
                         float leftLegX, float leftLegY,
                         float rightLegX, float rightLegY,
                         float armX, float armY,
                         float tailWaveTime, float flashWhite = 0.0f)
{
    const int W = 18;
    const int H = 24;
    float ps = scale;
    float dir = facingRight ? 1.0f : -1.0f;

    for (int r = 0; r < H; ++r)
    {
        for (int c = 0; c < W; ++c)
        {
            uint32_t val = grid[r][c];
            if (val == 0)
                continue;

            float rCol, gCol, bCol;
            unpackRGB(val, rCol, gCol, bCol);

            if (flashWhite > 0.0f)
            {
                rCol = rCol * (1.0f - flashWhite) + 1.0f * flashWhite;
                gCol = gCol * (1.0f - flashWhite) + 0.9f * flashWhite;
                bCol = bCol * (1.0f - flashWhite) + 0.9f * flashWhite;
            }

            // Local coordinates relative to character center-bottom
            int colIndex = facingRight ? c : (W - 1 - c);
            float px = static_cast<float>(colIndex - W / 2) * ps;
            float py = static_cast<float>(r - H + 1) * ps;

            // 1. Head & Hair (rows 0 to 11)
            if (r <= 11)
            {
                py += headY;
                px += torsoLean;
            }
            // 2. Torso, Robes & Belt (rows 12 to 16)
            else if (r <= 16)
            {
                py += torsoY;
                px += torsoLean * 0.5f;

                // Outer arm columns
                if (colIndex <= 3)
                {
                    px -= armX;
                    py += armY;
                }
                else if (colIndex >= 14)
                {
                    px += armX;
                    py -= armY;
                }
            }
            // 3. Legs / Boots (rows 17 to 23)
            else
            {
                if (tailWaveTime != 0.0f)
                {
                    // Serpentine traveling wave slither
                    float wave = std::sin(tailWaveTime - (r - 12) * 0.8f) * (r - 12) * 0.65f * ps;
                    px += (facingRight ? -wave : wave);
                }
                else
                {
                    // Bipedal Ninja Legs (Left: cols 3-8, Right: cols 9-14)
                    if (colIndex <= 8)
                    {
                        px += (facingRight ? leftLegX : -leftLegX);
                        py += leftLegY;
                    }
                    else
                    {
                        px += (facingRight ? rightLegX : -rightLegX);
                        py += rightLegY;
                    }
                }
            }

            M3Draw::quad(posX + px, posY + py, ps, ps, rCol, gCol, bCol, 1.0f);
        }
    }
}

// --------------------------------------------------------------------
// KAI - Fire Ninja (Melee: Flaming Sword of Fire Slash)
// --------------------------------------------------------------------
inline void drawKai(float x, float y, float scale, State state, float time, bool facingRight = true)
{
    float headY = 0.0f, torsoY = 0.0f, torsoLean = 0.0f;
    float lLegX = 0.0f, lLegY = 0.0f, rLegX = 0.0f, rLegY = 0.0f;
    float armX = 0.0f, armY = 0.0f;
    float dir = facingRight ? 1.0f : -1.0f;
    float flash = 0.0f;

    if (state == State::RUN)
    {
        // 4-Frame Aseprite Running Cycle
        int frame = static_cast<int>(time * 7.0f) % 4;
        if (frame == 0) // Right Stride
        {
            rLegX = 3.5f * scale; rLegY = -1.5f * scale;
            lLegX = -3.0f * scale; lLegY = 0.0f;
            torsoLean = 1.8f * dir * scale;
            armX = 2.5f * dir * scale;
            headY = 0.0f; torsoY = 0.0f;
        }
        else if (frame == 1) // Contact Squash (Heaviest step)
        {
            rLegX = 0.5f * scale; rLegY = 1.0f * scale;
            lLegX = -0.5f * scale; lLegY = 1.0f * scale;
            headY = 1.0f * scale; torsoY = 1.0f * scale;
            torsoLean = 0.5f * dir * scale;
            // Running dust puff
            M3Draw::quad(x - dir * 10.0f * scale, y - 2.0f * scale, 3.5f * scale, 2.5f * scale, 0.8f, 0.4f, 0.15f, 0.7f);
        }
        else if (frame == 2) // Left Stride
        {
            lLegX = 3.5f * scale; lLegY = -1.5f * scale;
            rLegX = -3.0f * scale; rLegY = 0.0f;
            torsoLean = 1.8f * dir * scale;
            armX = -2.5f * dir * scale;
            headY = 0.0f; torsoY = 0.0f;
        }
        else // Push-off Lift (Airborne stride)
        {
            lLegX = 0.5f * scale; lLegY = -1.0f * scale;
            rLegX = -0.5f * scale; rLegY = -1.0f * scale;
            headY = -1.0f * scale; torsoY = -1.0f * scale;
            torsoLean = 1.0f * dir * scale;
        }

        // Fire spark footsteps
        if (std::fmod(time, 0.2f) < 0.1f)
        {
            M3Draw::quad(x - dir * 12.0f * scale, y - 3.0f * scale, 2.5f * scale, 2.5f * scale, 1.0f, 0.6f, 0.1f, 0.9f);
        }
    }
    else if (state == State::JUMP)
    {
        torsoY = -18.0f * scale;
        headY = -18.0f * scale;
        lLegY = -14.0f * scale;
        rLegY = -14.0f * scale;
    }
    else if (state == State::DUCK)
    {
        headY = 6.0f * scale;
        torsoY = 6.0f * scale;
    }
    else if (state == State::ATTACK)
    {
        float cycle = std::fmod(time, 0.65f) / 0.65f;
        if (cycle < 0.20f) // Phase 0: Anticipation / Windup
        {
            torsoLean = -2.5f * dir * scale;
            torsoY = 2.0f * scale; headY = 2.0f * scale;
            // Sword drawn back, charging with fiery glow
            float hx = x - dir * 12.0f * scale;
            float hy = y - 18.0f * scale;
            M3Draw::line(x - dir * 4.0f * scale, y - 8.0f * scale, hx, hy, 4.0f * scale, 1.0f, 0.85f, 0.2f);
            M3Draw::circle(hx, hy, 6.0f * scale, 1.0f, 0.4f, 0.05f, 0.85f, 12);
        }
        else if (cycle < 0.55f) // Phase 1: Massive Forward Lunge & Fire Slash Arc!
        {
            float lunge = (cycle - 0.20f) / 0.35f;
            torsoLean = 8.0f * dir * scale;
            x += dir * 14.0f * scale;
            torsoY = 1.0f * scale;

            float arcProgress = lunge * 3.14159f;
            float swordTipX = x + dir * (26.0f + std::sin(arcProgress) * 14.0f) * scale;
            float swordTipY = y - (12.0f + std::cos(arcProgress) * 16.0f) * scale;

            // Sword of Fire Blade
            M3Draw::line(x + dir * 6.0f * scale, y - 8.0f * scale, swordTipX, swordTipY, 4.5f * scale, 1.0f, 0.95f, 0.8f);

            // Giant Crescent Fire Arc (3 Layered Waves)
            for (int f = 0; f < 7; ++f)
            {
                float fa = -1.2f + f * 0.42f;
                float dist = (34.0f + (f % 2) * 8.0f) * scale;
                float fx = x + dir * std::cos(fa) * dist;
                float fy = y - 10.0f * scale + std::sin(fa) * dist;
                float fSize = (7.5f - f * 0.6f) * scale;

                M3Draw::quad(fx - fSize * 0.5f, fy - fSize * 0.5f, fSize, fSize, 1.0f, 0.2f + f * 0.1f, 0.02f, 0.95f - f * 0.1f);
                M3Draw::quad(fx - fSize * 0.25f, fy - fSize * 0.25f, fSize * 0.5f, fSize * 0.5f, 1.0f, 0.95f, 0.4f, 0.95f);
            }
        }
        else // Phase 2: Recovery / Follow-Through
        {
            torsoLean = 3.0f * dir * scale;
            float swordTipX = x + dir * 20.0f * scale;
            float swordTipY = y - 6.0f * scale;
            M3Draw::line(x + dir * 6.0f * scale, y - 8.0f * scale, swordTipX, swordTipY, 3.5f * scale, 1.0f, 0.85f, 0.2f);
        }
    }

    renderSprite(CharacterData::SPRITE_KAI, x, y, scale, facingRight,
                 headY, torsoY, torsoLean, lLegX, lLegY, rLegX, rLegY, armX, armY, 0.0f, flash);
}

// --------------------------------------------------------------------
// ZANE - Ice Ninja (Ranged: Dual Frost Shuriken Throw)
// --------------------------------------------------------------------
inline void drawZane(float x, float y, float scale, State state, float time, bool facingRight = true)
{
    float headY = 0.0f, torsoY = 0.0f, torsoLean = 0.0f;
    float lLegX = 0.0f, lLegY = 0.0f, rLegX = 0.0f, rLegY = 0.0f;
    float armX = 0.0f, armY = 0.0f;
    float dir = facingRight ? 1.0f : -1.0f;

    if (state == State::RUN)
    {
        int frame = static_cast<int>(time * 7.0f) % 4;
        if (frame == 0)
        {
            rLegX = 3.5f * scale; rLegY = -1.5f * scale;
            lLegX = -3.0f * scale; lLegY = 0.0f;
            torsoLean = 1.5f * dir * scale;
            armX = 2.5f * dir * scale;
        }
        else if (frame == 1)
        {
            headY = 1.0f * scale; torsoY = 1.0f * scale;
            M3Draw::quad(x - dir * 10.0f * scale, y - 2.0f * scale, 3.5f * scale, 2.5f * scale, 0.35f, 0.8f, 0.95f, 0.7f);
        }
        else if (frame == 2)
        {
            lLegX = 3.5f * scale; lLegY = -1.5f * scale;
            rLegX = -3.0f * scale; rLegY = 0.0f;
            torsoLean = 1.5f * dir * scale;
            armX = -2.5f * dir * scale;
        }
        else
        {
            headY = -1.0f * scale; torsoY = -1.0f * scale;
        }

        // Frost particle footsteps
        if (std::fmod(time, 0.2f) < 0.1f)
        {
            M3Draw::quad(x - dir * 12.0f * scale, y - 3.0f * scale, 2.5f * scale, 2.5f * scale, 0.5f, 0.9f, 1.0f, 0.9f);
        }
    }
    else if (state == State::JUMP)
    {
        torsoY = -18.0f * scale; headY = -18.0f * scale;
    }
    else if (state == State::DUCK)
    {
        headY = 6.0f * scale; torsoY = 6.0f * scale;
    }
    else if (state == State::ATTACK)
    {
        float cycle = std::fmod(time, 0.70f) / 0.70f;
        if (cycle < 0.20f) // Windup
        {
            torsoLean = -2.0f * dir * scale;
            armX = -4.0f * dir * scale;
            // Frost aura charging
            M3Draw::circle(x - dir * 10.0f * scale, y - 12.0f * scale, 7.0f * scale, 0.4f, 0.88f, 1.0f, 0.8f, 12);
        }
        else // Whip arm forward & Launch Spinning Shurikens of Ice
        {
            torsoLean = 4.0f * dir * scale;
            armX = 6.0f * dir * scale;

            float flight = (cycle - 0.20f) / 0.80f;
            float projX = x + dir * (18.0f + flight * 280.0f) * scale;
            float projY = y - (12.0f - std::sin(flight * 3.14159f) * 10.0f) * scale;

            // Spinning 4-Point Shuriken
            float sSize = 7.0f * scale;
            M3Draw::quad(projX - sSize * 0.5f, projY - 1.5f * scale, sSize, 3.0f * scale, 0.9f, 0.98f, 1.0f);
            M3Draw::quad(projX - 1.5f * scale, projY - sSize * 0.5f, 3.0f * scale, sSize, 0.9f, 0.98f, 1.0f);
            M3Draw::circle(projX, projY, 2.8f * scale, 0.35f, 0.88f, 1.0f);

            // Frost trail streaks
            for (int t = 1; t <= 5; ++t)
            {
                float tx = projX - dir * t * 12.0f * scale;
                float ty = projY + (std::fmod(t * 7.0f, 5.0f) - 2.5f) * scale;
                float tSize = (4.5f - t * 0.7f) * scale;
                M3Draw::quad(tx, ty, tSize, tSize, 0.45f, 0.88f, 1.0f, 0.85f - t * 0.15f);
            }
        }
    }

    renderSprite(CharacterData::SPRITE_ZANE, x, y, scale, facingRight,
                 headY, torsoY, torsoLean, lLegX, lLegY, rLegX, rLegY, armX, armY, 0.0f);
}

// --------------------------------------------------------------------
// COLE - Earth Ninja (Heavy: Ground Quake Hammer Slam)
// --------------------------------------------------------------------
inline void drawCole(float x, float y, float scale, State state, float time, bool facingRight = true)
{
    float headY = 0.0f, torsoY = 0.0f, torsoLean = 0.0f;
    float lLegX = 0.0f, lLegY = 0.0f, rLegX = 0.0f, rLegY = 0.0f;
    float armX = 0.0f, armY = 0.0f;
    float dir = facingRight ? 1.0f : -1.0f;

    if (state == State::RUN)
    {
        int frame = static_cast<int>(time * 7.0f) % 4;
        if (frame == 0)
        {
            rLegX = 3.5f * scale; rLegY = -1.5f * scale;
            lLegX = -3.0f * scale; lLegY = 0.0f;
            torsoLean = 1.8f * dir * scale;
        }
        else if (frame == 1) // Heavy stomp
        {
            headY = 1.25f * scale; torsoY = 1.25f * scale;
            M3Draw::quad(x - dir * 10.0f * scale, y - 2.0f * scale, 4.5f * scale, 3.0f * scale, 0.45f, 0.38f, 0.3f, 0.8f);
        }
        else if (frame == 2)
        {
            lLegX = 3.5f * scale; lLegY = -1.5f * scale;
            rLegX = -3.0f * scale; rLegY = 0.0f;
            torsoLean = 1.8f * dir * scale;
        }
        else
        {
            headY = -1.0f * scale; torsoY = -1.0f * scale;
        }
    }
    else if (state == State::JUMP)
    {
        torsoY = -18.0f * scale; headY = -18.0f * scale;
    }
    else if (state == State::DUCK)
    {
        headY = 6.0f * scale; torsoY = 6.0f * scale;
    }
    else if (state == State::ATTACK)
    {
        float cycle = std::fmod(time, 0.80f) / 0.80f;
        if (cycle < 0.30f) // Windup: Raise Heavy Hammer Overhead
        {
            headY = -3.0f * scale; torsoY = -3.0f * scale;
            float hx = x - dir * 6.0f * scale;
            float hy = y - 30.0f * scale;
            M3Draw::line(x, y - 10.0f * scale, hx, hy, 4.5f * scale, 0.42f, 0.32f, 0.22f);
            M3Draw::quad(hx - 8.0f * scale, hy - 7.0f * scale, 16.0f * scale, 12.0f * scale, 0.32f, 0.30f, 0.34f);
            M3Draw::quad(hx - 6.0f * scale, hy - 5.0f * scale, 12.0f * scale, 8.0f * scale, 0.95f, 0.55f, 0.15f);
        }
        else if (cycle < 0.65f) // Impact: Slam Ground with Earth Shockwaves & Flying Rocks!
        {
            float slamProgress = (cycle - 0.30f) / 0.35f;
            float impactX = x + dir * 18.0f * scale;
            float impactY = y;

            M3Draw::line(x + dir * 4.0f * scale, y - 8.0f * scale, impactX, impactY, 5.0f * scale, 0.42f, 0.32f, 0.22f);
            M3Draw::quad(impactX - 9.0f * scale, impactY - 10.0f * scale, 18.0f * scale, 11.0f * scale, 0.32f, 0.30f, 0.34f);

            // Expanding Earth Shockwave Rings
            float rDist = (slamProgress * 42.0f + 6.0f) * scale;
            M3Draw::circle(impactX, impactY, rDist, 0.92f, 0.55f, 0.15f, 0.8f * (1.0f - slamProgress), 18);
            M3Draw::circle(impactX, impactY, rDist * 0.7f, 1.0f, 0.8f, 0.25f, 0.9f * (1.0f - slamProgress), 18);

            // Flying Rock Chunks
            for (int r = 0; r < 5; ++r)
            {
                float rx = impactX + dir * (r * 8.0f - 16.0f) * scale;
                float ry = impactY - (std::sin(slamProgress * 3.14159f) * (20.0f + (r % 3) * 10.0f)) * scale;
                float rSize = (4.5f + (r % 2) * 2.0f) * scale;
                M3Draw::quad(rx, ry, rSize, rSize, 0.42f, 0.38f, 0.35f, 0.95f);
            }
        }
        else
        {
            torsoLean = 2.0f * dir * scale;
        }
    }

    renderSprite(CharacterData::SPRITE_COLE, x, y, scale, facingRight,
                 headY, torsoY, torsoLean, lLegX, lLegY, rLegX, rLegY, armX, armY, 0.0f);
}

// --------------------------------------------------------------------
// MASTER WU - Sensei of the Monastery
// --------------------------------------------------------------------
inline void drawWu(float x, float y, float scale, State state, float time, bool facingRight = true)
{
    float headY = (state == State::RUN) ? std::sin(time * 8.0f) * 1.5f * scale : 0.0f;
    float dir = facingRight ? 1.0f : -1.0f;

    // Golden Dragon Staff
    float staffX = x + dir * 14.0f * scale;
    M3Draw::line(staffX, y - 24.0f * scale, staffX, y + 2.0f * scale, 3.0f * scale, 0.48f, 0.32f, 0.16f);
    M3Draw::circle(staffX, y - 24.0f * scale, 4.0f * scale, 0.98f, 0.82f, 0.25f);

    renderSprite(CharacterData::SPRITE_WU, x, y, scale, facingRight,
                 headY, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

// --------------------------------------------------------------------
// VENOMARI - Green Cobra Warrior (Villain: Fluid Slither & Acid Spit)
// --------------------------------------------------------------------
inline void drawVenomari(float x, float y, float scale, State state, float time, bool facingRight = false)
{
    float headY = 0.0f, torsoY = 0.0f, torsoLean = 0.0f;
    float dir = facingRight ? 1.0f : -1.0f;
    float tailTime = time * 7.0f;

    if (state == State::RUN)
    {
        headY = std::sin(time * 7.0f) * 1.0f * scale;
        torsoLean = 2.0f * dir * scale;

        // Dripping acid trail
        if (std::fmod(time, 0.25f) < 0.12f)
        {
            M3Draw::circle(x - dir * 10.0f * scale, y - 2.0f * scale, 2.5f * scale, 0.35f, 0.95f, 0.25f, 0.8f, 8);
        }
    }
    else if (state == State::ATTACK)
    {
        float cycle = std::fmod(time, 0.70f) / 0.70f;
        if (cycle < 0.25f) // Coils back, hood widens, fangs drip acid
        {
            torsoLean = -3.0f * dir * scale;
            headY = 2.0f * scale;
            M3Draw::circle(x + dir * 8.0f * scale, y - 10.0f * scale, 6.0f * scale, 0.35f, 0.98f, 0.2f, 0.85f, 10);
        }
        else // Lunges forward & Spits bubbling acid orb
        {
            torsoLean = 5.0f * dir * scale;
            float flight = (cycle - 0.25f) / 0.75f;
            float orbX = x + dir * (14.0f + flight * 260.0f) * scale;
            float orbY = y - (12.0f - std::sin(flight * 3.14159f) * 12.0f) * scale;

            // Green acid orb
            M3Draw::circle(orbX, orbY, 7.0f * scale, 0.3f, 0.95f, 0.2f, 0.95f, 12);
            M3Draw::circle(orbX, orbY, 4.0f * scale, 0.8f, 1.0f, 0.4f, 0.95f, 10);

            // Acid splatter droplets
            for (int s = 1; s <= 4; ++s)
            {
                float sx = orbX - dir * s * 8.0f * scale;
                float sy = orbY + std::sin(time * 20.0f + s) * 4.0f * scale;
                M3Draw::circle(sx, sy, (3.5f - s * 0.6f) * scale, 0.35f, 0.95f, 0.25f, 0.8f, 8);
            }
        }
    }

    renderSprite(CharacterData::SPRITE_VENOMARI, x, y, scale, facingRight,
                 headY, torsoY, torsoLean, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, tailTime);
}

// --------------------------------------------------------------------
// PYTHOR - Purple Anacondrai Serpent (Villain: Hypnotic Tail Whip)
// --------------------------------------------------------------------
inline void drawUploadedPythor(float x, float y, float scale, State state, float time, bool facingRight)
{
    // The uploaded PNG has transparent padding around the character, so use
    // a larger footprint to make its visible body match the green villain's
    // on-screen size.  The lower slices still use the same travelling-wave
    // timing as Venomari.
    constexpr float visibleSizeCompensation = 1.45f;
    const float displayScale = scale * visibleSizeCompensation;
    const float spriteW = 18.0f * displayScale;
    const float spriteH = 24.0f * displayScale;
    const float upperH = spriteH * 0.70f;
    const float top = y - spriteH;
    const bool waving = state == State::RUN || state == State::ATTACK;
    const float bob = waving ? std::sin(time * 7.0f) * 1.0f * displayScale : 0.0f;
    const float drawX = x - spriteW * 0.5f;
    const char* imagePath = "assets/textures/pythor_uploaded.bmp";

    M3Draw::imageRegion(imagePath, drawX, top + bob, spriteW, upperH,
                        facingRight ? 0.0f : 1.0f, 0.0f,
                        facingRight ? 1.0f : 0.0f, 0.70f);

    // Five lower slices reproduce the green villain's travelling tail wave:
    // each slice has a delayed phase and increasing displacement toward the
    // tail tip instead of moving the whole purple character as one block.
    constexpr int tailSlices = 5;
    const float tailH = spriteH - upperH;
    const float sliceH = tailH / static_cast<float>(tailSlices);
    for (int slice = 0; slice < tailSlices; ++slice)
    {
        const float v0 = 0.70f + 0.30f * (static_cast<float>(slice) / tailSlices);
        const float v1 = 0.70f + 0.30f * (static_cast<float>(slice + 1) / tailSlices);
        const float wave = waving
                               ? std::sin(time * 7.0f - (slice + 5) * 0.8f) *
                                     (slice + 5) * 0.65f * displayScale
                               : 0.0f;
        M3Draw::imageRegion(imagePath, drawX + wave,
                            top + upperH + slice * sliceH + bob,
                            spriteW, sliceH,
                            facingRight ? 0.0f : 1.0f, v0,
                            facingRight ? 1.0f : 0.0f, v1);
    }
}

inline void drawPythor(float x, float y, float scale, State state, float time, bool facingRight = false)
{
    drawUploadedPythor(x, y, scale, state, time, facingRight);
}

} // namespace CharacterSprites
