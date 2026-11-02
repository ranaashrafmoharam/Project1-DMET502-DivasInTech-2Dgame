#include "EnvironmentElements.h"
// ====================================================================
// FILE:     src/scene/EnvironmentElements.cpp
// OWNER:    Member 3
// TASKS:    M3-03 to M3-07, US-52, US-53
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Authentic Game Boy / 16-bit Pokemon style Ninjago environment elements:
//   1. Grand Vermilion Torii Gate with Shimenawa rope & Shide streamers
//   2. Multi-tier Monastery Pagoda with curved eaves and golden crest
//   3. Four Elemental Ninja Clan Banners (Fire, Lightning, Ice, Earth)
//   4. Granite Stone Lanterns (Toro) with warm flickering fire
//   5. Martial Arts Training Dummy (Muk Yan Jong) and weapon stand
//   6. Ancient Sakura Cherry Blossom Tree with lush petal canopy
// ====================================================================

#include "Scene.h"
#include <cmath>
#include <initializer_list>

// =========================================================================
// 1. GRAND VERMILION TORII GATE (Right side spawn portal: x = 1110 to 1250)
// =========================================================================
void EnvironmentElements::drawElement1()
{
    const float bx = 1115.0f;
    const float by = 265.0f;

    // Stone base plinths (Kamebara)
    M3Draw::quad(bx + 12, by + 105, 24, 20, 0.22f, 0.22f, 0.24f);
    M3Draw::quad(bx + 102, by + 105, 24, 20, 0.22f, 0.22f, 0.24f);
    M3Draw::quad(bx + 10, by + 118, 28, 7, 0.16f, 0.16f, 0.18f);
    M3Draw::quad(bx + 100, by + 118, 28, 7, 0.16f, 0.16f, 0.18f);

    // Left Vermilion Pillar (Hashira)
    M3Draw::quad(bx + 16, by + 22, 16, 85, 0.78f, 0.16f, 0.10f);
    M3Draw::quad(bx + 16, by + 22, 4, 85, 0.92f, 0.28f, 0.16f); // Left highlight
    M3Draw::quad(bx + 28, by + 22, 4, 85, 0.55f, 0.10f, 0.08f); // Right shadow
    // Gold collar rings
    M3Draw::quad(bx + 14, by + 30, 20, 5, 0.94f, 0.78f, 0.22f);

    // Right Vermilion Pillar
    M3Draw::quad(bx + 106, by + 22, 16, 85, 0.78f, 0.16f, 0.10f);
    M3Draw::quad(bx + 106, by + 22, 4, 85, 0.92f, 0.28f, 0.16f);
    M3Draw::quad(bx + 118, by + 22, 4, 85, 0.55f, 0.10f, 0.08f);
    M3Draw::quad(bx + 104, by + 30, 20, 5, 0.94f, 0.78f, 0.22f);

    // Horizontal tie-beam (Nuki)
    M3Draw::quad(bx, by + 34, 138, 10, 0.72f, 0.15f, 0.10f);
    M3Draw::quad(bx, by + 34, 138, 2, 0.88f, 0.26f, 0.16f);

    // Main curved top lintel (Kasagi & Shimaki)
    M3Draw::quad(bx - 12, by + 6, 162, 14, 0.78f, 0.16f, 0.10f);
    M3Draw::quad(bx - 18, by - 4, 174, 12, 0.14f, 0.12f, 0.15f); // Black curved roof
    M3Draw::quad(bx - 18, by - 4, 174, 2, 0.35f, 0.32f, 0.38f);
    // Gold end accents
    M3Draw::quad(bx - 20, by - 6, 6, 15, 0.95f, 0.80f, 0.24f);
    M3Draw::quad(bx + 152, by - 6, 6, 15, 0.95f, 0.80f, 0.24f);

    // Center tablet strut (Gakuzuka) with golden dragon character
    M3Draw::quad(bx + 62, by + 18, 14, 18, 0.14f, 0.12f, 0.15f);
    M3Draw::quad(bx + 65, by + 21, 8, 12, 0.95f, 0.82f, 0.24f);

    // Sacred woven rope (Shimenawa) and hanging white paper streamers (Shide)
    M3Draw::line(bx + 30, by + 46, bx + 108, by + 46, 5, 0.82f, 0.70f, 0.44f);
    for (float sx : {bx + 44.0f, bx + 68.0f, bx + 92.0f})
    {
        // Zigzag paper streamers
        M3Draw::quad(sx, by + 49, 7, 7, 0.96f, 0.96f, 0.94f);
        M3Draw::quad(sx + 3, by + 56, 7, 7, 0.96f, 0.96f, 0.94f);
        M3Draw::quad(sx + 1, by + 63, 6, 6, 0.96f, 0.96f, 0.94f);
    }
}

// =========================================================================
// 2. MULTI-TIER MONASTERY PAGODA (Background center: x = 400 to 760)
// =========================================================================
void EnvironmentElements::drawElement2()
{
    const float px = 560.0f;
    const float py = 390.0f;

    // Monastery stone podium
    M3Draw::quad(px - 170, py - 20, 340, 20, 0.28f, 0.26f, 0.27f);
    M3Draw::quad(px - 170, py - 20, 340, 3, 0.42f, 0.40f, 0.40f);

    // Wall structure with red pillars and white sliding screens
    M3Draw::quad(px - 150, py - 70, 300, 50, 0.88f, 0.86f, 0.80f); // Paper walls
    // Red structural timber beams
    for (int col = 0; col < 9; ++col)
    {
        float cx = px - 150 + col * 37.5f;
        M3Draw::quad(cx, py - 70, 5, 50, 0.65f, 0.16f, 0.12f);
    }
    // Horizontal mullions
    M3Draw::quad(px - 150, py - 46, 300, 4, 0.65f, 0.16f, 0.12f);

    // --- Tier 1 Roof (Lower Eave) ---
    M3Draw::quad(px - 180, py - 85, 360, 15, 0.16f, 0.18f, 0.22f);
    M3Draw::quad(px - 195, py - 95, 390, 12, 0.12f, 0.14f, 0.18f);
    M3Draw::quad(px - 195, py - 95, 390, 3, 0.26f, 0.30f, 0.38f); // Roof highlight
    // Upturned corners
    M3Draw::triangle(px - 210, py - 105, px - 195, py - 95, px - 195, py - 83, 0.12f, 0.14f, 0.18f);
    M3Draw::triangle(px + 210, py - 105, px + 195, py - 95, px + 195, py - 83, 0.12f, 0.14f, 0.18f);

    // --- Tier 2 Wall & Balcony ---
    M3Draw::quad(px - 110, py - 130, 220, 35, 0.88f, 0.86f, 0.80f);
    for (int col = 0; col < 7; ++col)
    {
        float cx = px - 110 + col * 36.0f;
        M3Draw::quad(cx, py - 130, 5, 35, 0.65f, 0.16f, 0.12f);
    }

    // --- Tier 2 Roof (Middle Eave) ---
    M3Draw::quad(px - 135, py - 142, 270, 12, 0.16f, 0.18f, 0.22f);
    M3Draw::quad(px - 148, py - 152, 296, 11, 0.12f, 0.14f, 0.18f);
    M3Draw::quad(px - 148, py - 152, 296, 3, 0.26f, 0.30f, 0.38f);
    M3Draw::triangle(px - 162, py - 160, px - 148, py - 152, px - 148, py - 141, 0.12f, 0.14f, 0.18f);
    M3Draw::triangle(px + 162, py - 160, px + 148, py - 152, px + 148, py - 141, 0.12f, 0.14f, 0.18f);

    // --- Tier 3 Roof (Grand Top Pagoda Roof) ---
    M3Draw::quad(px - 85, py - 180, 170, 28, 0.65f, 0.16f, 0.12f);
    M3Draw::quad(px - 110, py - 192, 220, 13, 0.16f, 0.18f, 0.22f);
    M3Draw::quad(px - 125, py - 204, 250, 12, 0.12f, 0.14f, 0.18f);
    M3Draw::quad(px - 125, py - 204, 250, 3, 0.26f, 0.30f, 0.38f);
    M3Draw::triangle(px - 138, py - 212, px - 125, py - 204, px - 125, py - 192, 0.12f, 0.14f, 0.18f);
    M3Draw::triangle(px + 138, py - 212, px + 125, py - 204, px + 125, py - 192, 0.12f, 0.14f, 0.18f);

    // Golden Roof Ridge Ornament (Dragon Shachihoko & Spire)
    M3Draw::quad(px - 14, py - 214, 28, 10, 0.95f, 0.78f, 0.22f);
    M3Draw::quad(px - 4, py - 232, 8, 18, 0.95f, 0.78f, 0.22f);
    M3Draw::circle(px, py - 234, 6, 1.0f, 0.88f, 0.36f);
    // Dragon horns on ridge ends
    M3Draw::triangle(px - 120, py - 204, px - 125, py - 215, px - 110, py - 204, 0.95f, 0.78f, 0.22f);
    M3Draw::triangle(px + 120, py - 204, px + 125, py - 215, px + 110, py - 204, 0.95f, 0.78f, 0.22f);
}

// =========================================================================
// 3. FOUR NINJAGO CLAN BANNERS (Fire, Lightning, Ice, Earth)
// =========================================================================
void EnvironmentElements::drawElement3()
{
    float t = M3Draw::getTime();
    struct BannerDef
    {
        float x;
        float r, g, b;       // Primary color
        float cr, cg, cb;    // Crest color
        const char* symbol;
    };

    const BannerDef banners[3] = {
        {880.0f,  0.86f, 0.16f, 0.14f, 0.98f, 0.86f, 0.24f, "FIRE"},      // Kai
        {935.0f,  0.85f, 0.92f, 0.96f, 0.28f, 0.75f, 0.88f, "ICE"},       // Zane; blue banner removed
        {990.0f,  0.24f, 0.22f, 0.20f, 0.92f, 0.54f, 0.14f, "EARTH"}      // Cole; shifted left
    };

    for (int i = 0; i < 3; ++i)
    {
        const BannerDef& b = banners[i];
        float bx = b.x;
        float by = 265.0f;

        // Wooden pole with bronze cap
        M3Draw::quad(bx, by, 5, 125, 0.36f, 0.22f, 0.14f);
        M3Draw::quad(bx - 2, by, 9, 6, 0.78f, 0.65f, 0.30f);
        // Golden spearhead finial
        M3Draw::triangle(bx - 3, by, bx + 2.5f, by - 14, bx + 8, by, 0.95f, 0.82f, 0.24f);

        // Horizontal crossbar
        M3Draw::quad(bx + 4, by + 12, 34, 4, 0.36f, 0.22f, 0.14f);

        // Hanging waving cloth banner
        float wave = std::sin(t * 3.5f + i * 1.2f) * 3.0f;
        float bannerW = 30.0f;
        float bannerH = 75.0f;

        // Main banner body with gentle wave
        M3Draw::quad(bx + 6 + wave * 0.4f, by + 16, bannerW, bannerH, b.r, b.g, b.b);
        // Shadow crease
        M3Draw::quad(bx + 6 + wave * 0.4f, by + 16, 4, bannerH, b.r * 0.7f, b.g * 0.7f, b.b * 0.7f);

        // Swallowtail bottom cut
        float botY = by + 16 + bannerH;
        M3Draw::triangle(bx + 6 + wave * 0.4f, botY,
                         bx + 6 + bannerW * 0.5f + wave * 0.4f, botY - 12,
                         bx + 6 + bannerW + wave * 0.4f, botY,
                         0.20f, 0.14f, 0.31f); // Sky behind cutout

        // Golden elemental emblem in center of banner
        float emX = bx + 6 + bannerW * 0.5f + wave * 0.3f;
        float emY = by + 46.0f;
        M3Draw::circle(emX, emY, 8.0f, b.cr, b.cg, b.cb);
        M3Draw::circle(emX, emY, 4.0f, b.r, b.g, b.b);
    }
}

// =========================================================================
// 4. GRANITE STONE LANTERNS (Toro) WITH FLICKERING FLAME
// =========================================================================
void EnvironmentElements::drawElement4(bool includeLeftLantern)
{
    float t = M3Draw::getTime();

    for (int lanternIndex = 0; lanternIndex < 2; ++lanternIndex)
    {
        if (lanternIndex == 0 && !includeLeftLantern)
            continue;

        const float lx = lanternIndex == 0 ? 215.0f : 1085.0f;
        float ly = 320.0f;

        // Base foundation stone (Kiso)
        M3Draw::quad(lx - 20, ly + 58, 40, 12, 0.32f, 0.32f, 0.34f);
        M3Draw::quad(lx - 24, ly + 66, 48,  6, 0.24f, 0.24f, 0.26f);

        // Stone pillar shaft (Sao)
        M3Draw::quad(lx - 8, ly + 25, 16, 33, 0.38f, 0.38f, 0.40f);
        M3Draw::quad(lx - 8, ly + 25,  4, 33, 0.50f, 0.50f, 0.54f); // Highlight

        // Middle stone plate (Chūdai)
        M3Draw::quad(lx - 16, ly + 18, 32, 7, 0.30f, 0.30f, 0.32f);

        // Carved light chamber (Hibukuro)
        M3Draw::quad(lx - 14, ly - 2, 28, 20, 0.20f, 0.20f, 0.22f);
        // Lattice corner posts
        M3Draw::quad(lx - 14, ly - 2,  4, 20, 0.35f, 0.35f, 0.38f);
        M3Draw::quad(lx + 10, ly - 2,  4, 20, 0.35f, 0.35f, 0.38f);

        // Warm flickering candlelight inside
        float flicker = 0.85f + 0.15f * std::sin(t * 14.0f + lx * 0.05f)
                              + 0.10f * std::cos(t * 22.0f);
        // Fire glow halo
        M3Draw::circle(lx, ly + 8, 14.0f * flicker, 0.98f, 0.65f, 0.14f, 0.35f * flicker, 16);
        // Fire core
        M3Draw::quad(lx - 4, ly + 4, 8, 9, 1.0f, 0.85f, 0.28f, flicker);
        M3Draw::quad(lx - 2, ly + 2, 4, 6, 1.0f, 0.98f, 0.65f, flicker);

        // Curved hexagonal stone roof (Kasa)
        M3Draw::quad(lx - 22, ly - 10, 44, 8, 0.28f, 0.28f, 0.30f);
        M3Draw::quad(lx - 16, ly - 16, 32, 6, 0.34f, 0.34f, 0.36f);
        M3Draw::triangle(lx - 26, ly - 10, lx - 18, ly - 16, lx - 16, ly - 10, 0.28f, 0.28f, 0.30f);
        M3Draw::triangle(lx + 26, ly - 10, lx + 18, ly - 16, lx + 16, ly - 10, 0.28f, 0.28f, 0.30f);

        // Jewel stone finial (Hōju)
        M3Draw::circle(lx, ly - 20, 5, 0.42f, 0.42f, 0.45f);
    }
}

// =========================================================================
// 5. MARTIAL ARTS TRAINING DUMMY & WEAPON RACK
// =========================================================================
void EnvironmentElements::drawElement5()
{
    const float dx = 320.0f;
    const float dy = 385.0f;

    // The training dummy beside the left sakura was removed from the preview.
    // Keep the nearby Ninja Weapon Rack (Katana display stand).
    const float rx = 360.0f;
    M3Draw::quad(rx, dy - 45, 4, 45, 0.18f, 0.14f, 0.12f);
    M3Draw::quad(rx + 36, dy - 45, 4, 45, 0.18f, 0.14f, 0.12f);
    M3Draw::quad(rx - 4, dy - 4, 48, 5, 0.22f, 0.16f, 0.14f);

    // Katana 1 (Top rack)
    M3Draw::line(rx - 8, dy - 36, rx + 44, dy - 36, 4, 0.88f, 0.90f, 0.94f); // Silver blade
    M3Draw::quad(rx - 8, dy - 38, 10, 8, 0.85f, 0.20f, 0.15f);              // Red wrapped handle
    M3Draw::quad(rx + 2, dy - 39,  3, 10, 0.95f, 0.78f, 0.22f);             // Gold tsuba guard

    // Katana 2 (Lower rack)
    M3Draw::line(rx - 6, dy - 20, rx + 42, dy - 20, 4, 0.88f, 0.90f, 0.94f);
    M3Draw::quad(rx - 6, dy - 22, 10, 8, 0.15f, 0.35f, 0.75f);              // Blue handle
    M3Draw::quad(rx + 4, dy - 23,  3, 10, 0.95f, 0.78f, 0.22f);
}

// =========================================================================
// 6. ANCIENT SAKURA (CHERRY BLOSSOM) TREE (x = 240 to 320)
// =========================================================================
void EnvironmentElements::drawElement6()
{
    const float t = M3Draw::getTime();

    auto drawSakuraTree = [&](float tx, float ty, float phase) {
        // The canopy and branches sway together, with the outer blossoms
        // moving farther than the trunk for a clear, readable wind motion.
        const float trunkSway = std::sin(t * 2.2f + phase) * 2.5f;
        const float canopySway = std::sin(t * 2.2f + phase) * 8.0f;

        // Gnarled tree trunk with branches
        M3Draw::quad(tx - 10 + trunkSway, ty - 60, 20, 60, 0.32f, 0.18f, 0.11f);
        M3Draw::quad(tx - 10 + trunkSway, ty - 60, 5, 60, 0.44f, 0.26f, 0.16f);
        // Spreading roots
        M3Draw::triangle(tx - 10 + trunkSway, ty - 20, tx - 28, ty,
                         tx - 10 + trunkSway, ty, 0.28f, 0.16f, 0.10f);
        M3Draw::triangle(tx + 10 + trunkSway, ty - 20, tx + 24, ty,
                         tx + 10 + trunkSway, ty, 0.28f, 0.16f, 0.10f);

        // Heavy gnarled branches stretching left and right
        M3Draw::quad(tx - 36 + trunkSway, ty - 75, 30, 14, 0.32f, 0.18f, 0.11f);
        M3Draw::quad(tx + 6 + trunkSway, ty - 85, 38, 14, 0.32f, 0.18f, 0.11f);
        M3Draw::quad(tx - 8 + trunkSway, ty - 100, 16, 25, 0.32f, 0.18f, 0.11f);

        auto drawPetalCluster = [&](float cx, float cy, float r, float amount) {
            const float sx = cx + canopySway * amount;
            M3Draw::circle(sx, cy, r, 0.82f, 0.38f, 0.56f, 1.0f, 20);
            M3Draw::circle(sx - 3, cy - 3, r * 0.82f, 0.96f, 0.58f, 0.74f, 1.0f, 18);
            M3Draw::circle(sx - 6, cy - 6, r * 0.50f, 1.0f, 0.85f, 0.92f, 1.0f, 14);
        };

        drawPetalCluster(tx - 45, ty - 80, 24, 0.70f);
        drawPetalCluster(tx + 45, ty - 90, 26, 1.00f);
        drawPetalCluster(tx - 15, ty - 110, 28, 0.85f);
        drawPetalCluster(tx + 20, ty - 115, 25, 0.95f);
        drawPetalCluster(tx, ty - 90, 30, 0.80f);
    };

    // Original tree and a matching second tree in the marked right-side
    // background area.  Their phase offsets make the shared wind visible.
    drawSakuraTree(325.0f, 390.0f, 0.0f);
    drawSakuraTree(800.0f, 390.0f, 1.25f);
}
