#include "HomeBase.h"
// ====================================================================
// FILE:     src/scene/HomeBase.cpp
// OWNER:    Member 3
// TASKS:    M3-08, US-12, US-51, US-53
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   The player's safe zone at the left end of the playfield:
//   The fortified Monastery of Spinjitzu Gate with the Golden Weapons
//   Crest, studded timber double doors, red pillars and the defense line.
//
// RESPONSIBILITIES:
//   - Detailed structure clearly different from environment
//   - Its right edge (x = 252) is the losing line used by Member 1
//   - Highly visible against the background in classic retro RPG pixel art
// ====================================================================

#include "Scene.h"
#include <cmath>
#include <initializer_list>

void HomeBase::draw()
{
    // Extend only the left side of the home base to close the gray gap.
    // The right edge remains at x=252 so the gameplay lanes are unchanged.
    const float bx = 0.0f;
    const float by = 390.0f;
    const float bw = 252.0f;
    const float bh = 330.0f;
    float t = M3Draw::getTime();

    // =========================================================================
    // 1. GATEHOUSE ROOF (Overhanging pagoda roof above the gate: y = 280 to 390)
    // =========================================================================
    // Eaves brackets & timber support
    M3Draw::quad(bx, by - 24, bw - 12, 24, 0.42f, 0.22f, 0.12f);
    for (int b = 0; b < 10; ++b)
    {
        float brX = bx + 24 + b * 20.0f;
        M3Draw::quad(brX, by - 28, 6, 28, 0.55f, 0.30f, 0.16f);
    }

    // Lower eave roof tiles (dark charcoal slate)
    M3Draw::quad(bx - 10, by - 38, bw + 20, 14, 0.16f, 0.17f, 0.22f);
    M3Draw::quad(bx - 16, by - 50, bw + 32, 13, 0.12f, 0.13f, 0.18f);
    M3Draw::quad(bx - 16, by - 50, bw + 32,  3, 0.28f, 0.32f, 0.40f); // Roof ridge highlight
    // Upturned corners
    M3Draw::triangle(bx - 26, by - 58, bx - 16, by - 50, bx - 16, by - 38, 0.12f, 0.13f, 0.18f);
    M3Draw::triangle(bx + bw + 26, by - 58, bx + bw + 16, by - 50, bx + bw + 16, by - 38, 0.12f, 0.13f, 0.18f);

    // Upper grand eave (Tier 2)
    M3Draw::quad(bx,      by - 70, bw - 20, 22, 0.65f, 0.16f, 0.12f); // Red wall
    M3Draw::quad(bx,      by - 82, bw - 6,  13, 0.16f, 0.17f, 0.22f);
    M3Draw::quad(bx,      by - 94, bw,      13, 0.12f, 0.13f, 0.18f);
    M3Draw::quad(bx,      by - 94, bw,       3, 0.28f, 0.32f, 0.40f);
    M3Draw::triangle(bx - 12, by - 102, bx, by - 94, bx, by - 82, 0.12f, 0.13f, 0.18f);
    M3Draw::triangle(bx + bw + 12, by - 102, bx + bw, by - 94, bx + bw, by - 82, 0.12f, 0.13f, 0.18f);

    // Golden Dragon Crest atop the Monastery Gate, treated as a warm torch
    // so its light visibly flickers like the scene lanterns.
    const float crestX = bx + bw * 0.5f;
    const float crestY = by - 106.0f;
    const float crestFlicker = 0.88f + 0.12f * std::sin(t * 14.0f + 0.7f)
                               + 0.06f * std::cos(t * 22.0f);
    M3Draw::circle(crestX, crestY + 3.0f, 20.0f * crestFlicker,
                   0.98f, 0.62f, 0.12f, 0.18f * crestFlicker, 16);
    M3Draw::quad(crestX - 18, crestY, 36, 14, 0.95f, 0.78f, 0.22f);
    M3Draw::circle(crestX, crestY, 8.0f + crestFlicker * 1.5f,
                   1.0f, 0.92f, 0.44f, crestFlicker);
    M3Draw::triangle(bx + bw * 0.5f - 6, by - 106, bx + bw * 0.5f, by - 122, bx + bw * 0.5f + 6, by - 106, 0.95f, 0.78f, 0.22f);

    // =========================================================================
    // 2. MONASTERY STONE FOUNDATION & WALL MASONRY (y = 390 to 720)
    // =========================================================================
    // Base stone blocks
    M3Draw::quad(bx, by, bw, bh, 0.22f, 0.20f, 0.22f);
    // Stone masonry pattern
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 6; ++col)
        {
            float mx = bx + 10 + col * 36.0f + (row % 2 ? 18.0f : 0.0f);
            float my = by + 12 + row * 38.0f;
            if (mx + 30 <= bx + bw - 10)
            {
                M3Draw::quad(mx, my, 32, 18, 0.34f, 0.32f, 0.34f);
                M3Draw::quad(mx, my, 32, 2, 0.46f, 0.44f, 0.46f); // Stone highlight
                M3Draw::quad(mx, my + 16, 32, 2, 0.16f, 0.15f, 0.16f); // Stone shadow
            }
        }
    }

    // =========================================================================
    // 3. FORTIFIED DOUBLE TIMBER GATE DOORS (The Monastery Entrance)
    // =========================================================================
    // Keep the gate itself in its original position while adding the new
    // structure on the left side.
    const float gx = 66.0f;
    const float gw = 120.0f;
    // Move the complete gate assembly down so its bottom edge meets the
    // foundation ground line with no visible gap.
    const float gy = by + 35.0f;
    const float gh = bh - 35.0f;

    // Dark gate archway depth
    M3Draw::quad(gx - 8, gy - 8, gw + 16, gh + 8, 0.10f, 0.08f, 0.09f);

    // Heavy dark wood gate panels (Left & Right)
    M3Draw::quad(gx, gy, gw * 0.5f - 2, gh, 0.36f, 0.20f, 0.12f);
    M3Draw::quad(gx + gw * 0.5f + 2, gy, gw * 0.5f - 2, gh, 0.36f, 0.20f, 0.12f);

    // Vertical timber plank lines
    for (float px = gx + 14; px < gx + gw; px += 28)
    {
        M3Draw::line(px, gy, px, gy + gh, 2, 0.24f, 0.12f, 0.08f);
    }

    // Heavy horizontal iron reinforcing bands
    for (float iy : {gy + 40.0f, gy + 120.0f, gy + 200.0f})
    {
        M3Draw::quad(gx - 2, iy, gw + 4, 12, 0.18f, 0.18f, 0.20f);
        M3Draw::quad(gx - 2, iy, gw + 4,  2, 0.35f, 0.35f, 0.38f); // Iron highlight
        // Iron rivets/studs
        for (float rx = gx + 10; rx < gx + gw; rx += 22)
        {
            M3Draw::quad(rx, iy + 3, 5, 5, 0.50f, 0.50f, 0.55f);
        }
    }

    // Large bronze dragon ring door knockers
    for (float kx : {gx + gw * 0.28f, gx + gw * 0.72f})
    {
        float ky = gy + 140.0f;
        M3Draw::quad(kx - 7, ky - 7, 14, 14, 0.85f, 0.68f, 0.22f); // Base plate
        M3Draw::circle(kx, ky + 10, 9, 0.78f, 0.60f, 0.18f, 1.0f, 14); // Ring
        M3Draw::circle(kx, ky + 10, 5, 0.28f, 0.16f, 0.10f, 1.0f, 10); // Ring hole
    }

    // =========================================================================
    // 4. VERMILION GATE POSTS & GOLDEN WEAPONS EMBLEM
    // =========================================================================
    // Left vermilion pillar
    M3Draw::quad(gx - 18, gy - 12, 16, gh + 12, 0.78f, 0.16f, 0.10f);
    M3Draw::quad(gx - 18, gy - 12,  4, gh + 12, 0.92f, 0.28f, 0.16f);
    // Right vermilion pillar
    M3Draw::quad(gx + gw + 2, gy - 12, 16, gh + 12, 0.78f, 0.16f, 0.10f);
    M3Draw::quad(gx + gw + 2, gy - 12,  4, gh + 12, 0.92f, 0.28f, 0.16f);

    // Sacred Golden Weapons Crest Plaque mounted above the doors
    float plaqueX = gx + gw * 0.5f;
    float plaqueY = gy - 18.0f;
    M3Draw::quad(plaqueX - 32, plaqueY - 14, 64, 28, 0.14f, 0.12f, 0.15f); // Black lacquered plate
    M3Draw::quad(plaqueX - 30, plaqueY - 12, 60, 24, 0.85f, 0.70f, 0.22f); // Gold plate rim
    M3Draw::quad(plaqueX - 28, plaqueY - 10, 56, 20, 0.18f, 0.14f, 0.16f);
    // Crossed Golden Weapons symbol in center
    M3Draw::line(plaqueX - 16, plaqueY - 6, plaqueX + 16, plaqueY + 6, 3, 1.0f, 0.88f, 0.25f);
    M3Draw::line(plaqueX - 16, plaqueY + 6, plaqueX + 16, plaqueY - 6, 3, 1.0f, 0.88f, 0.25f);
    M3Draw::circle(plaqueX, plaqueY, 5, 1.0f, 0.95f, 0.45f);

    // =========================================================================
    // 5. HANGING RED MONASTERY LANTERNS
    // =========================================================================
    for (float lx : {gx - 28.0f, gx + gw + 28.0f})
    {
        float ly = gy + 30.0f;
        float flicker = 0.85f + 0.15f * std::sin(t * 14.0f + lx * 0.05f)
                              + 0.10f * std::cos(t * 22.0f);
        // Visible warm halo and a pulsing core for each hanging torch.
        M3Draw::circle(lx, ly + 13.0f, 22.0f * flicker,
                       0.98f, 0.55f, 0.10f, 0.18f * flicker, 16);
        // Chain
        M3Draw::line(lx, ly - 18, lx, ly, 2, 0.18f, 0.18f, 0.20f);
        // Lantern body (curved red paper with black ribs)
        M3Draw::quad(lx - 10, ly, 20, 28, 0.88f, 0.18f, 0.14f);
        M3Draw::quad(lx - 8, ly, 16, 28, 1.00f, 0.35f, 0.20f * flicker, flicker); // Center glow
        M3Draw::quad(lx - 3, ly + 5, 6, 12, 1.00f, 0.92f, 0.38f, flicker); // Flame core
        M3Draw::quad(lx - 12, ly - 3, 24, 4, 0.16f, 0.14f, 0.16f); // Cap
        M3Draw::quad(lx - 12, ly + 27, 24, 4, 0.16f, 0.14f, 0.16f); // Base
        // Golden tassel
        M3Draw::line(lx, ly + 31, lx, ly + 42, 3, 0.95f, 0.82f, 0.25f);
    }

    // =========================================================================
    // 6. DEFENSE LOSING BOUNDARY LINE (kept at the original lane boundary)
    // =========================================================================
    // Glowing golden defensive ward with subtle energy pulse
    float pulse = 0.80f + 0.20f * std::sin(t * 5.0f);
    // Outer golden glow line
    const float losingLine = 252.0f;
    M3Draw::line(losingLine, by, losingLine, by + bh, 7.0f, 0.95f, 0.78f, 0.20f, 0.35f * pulse);
    // Core golden barrier line
    M3Draw::line(losingLine, by, losingLine, by + bh, 3.0f, 1.00f, 0.92f, 0.45f, 0.90f * pulse);

    // Decorative dragon ward rune notches along the line
    for (int notch = 0; notch < 6; ++notch)
    {
        float ny = by + 25.0f + notch * 52.0f;
        M3Draw::quad(losingLine - 4.0f, ny, 8.0f, 8.0f, 1.00f, 0.85f, 0.30f, 0.85f * pulse);
        M3Draw::quad(losingLine - 2.0f, ny + 2, 4.0f, 4.0f, 1.00f, 1.00f, 0.85f, pulse);
    }
}

HomeBaseBounds HomeBase::getBounds() const
{
    return {0.0f, 252.0f, 390.0f, 720.0f};
}
