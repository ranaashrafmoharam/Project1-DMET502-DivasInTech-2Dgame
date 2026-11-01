#pragma once
// ====================================================================
// FILE:     src/core/Constants.h
// Single source of truth for shared constants across the game.
// ====================================================================

namespace Constants {
    // Window & Viewport
    constexpr float WINDOW_WIDTH  = 1280.0f;
    constexpr float WINDOW_HEIGHT = 720.0f;

    // Grid and Lanes
    constexpr int   NUM_LANES        = 4;
    constexpr float LANE_HEIGHT      = 120.0f;
    constexpr float FIELD_Y0         = 60.0f;
    constexpr float FIELD_X0         = 160.0f;  // Home base boundary line
    constexpr float FIELD_X1         = 1240.0f; // Right spawn boundary
    constexpr float HOME_BASE_WIDTH  = 160.0f;

    inline float laneY(int lane) {
        // Lane 0 is top lane, Lane NUM_LANES-1 is bottom lane
        return FIELD_Y0 + (NUM_LANES - 1 - lane) * LANE_HEIGHT;
    }

    // Currency & Card Costs (US-22)
    constexpr int STARTING_COINS  = 30;
    constexpr int COST_MELEE      = 10;
    constexpr int COST_RANGED     = 20; // 2x Melee
    constexpr int COST_DEFENSE    = 30; // 3x Melee
    constexpr int SHARD_VALUE     = 10;

    // Movement Speeds
    constexpr float SPEED_PLAYER        = 55.0f;
    constexpr float SPEED_ENEMY_NORMAL  = 50.0f;
    constexpr float SPEED_ENEMY_FAST    = 85.0f; // Hard enemy runs faster (US-37)
    constexpr float PROJECTILE_SPEED    = 360.0f;

    // Durability & Health (PDF stats table)
    constexpr int HEALTH_MELEE_PLAYER   = 5;
    constexpr int HEALTH_RANGED_PLAYER  = 5;
    constexpr int HEALTH_DEFENSE_PLAYER = 7;

    constexpr int HEALTH_EASY_ENEMY     = 5;
    constexpr int HEALTH_MEDIUM_ENEMY   = 5;
    constexpr int HEALTH_HARD_ENEMY     = 7;

    // Damage Dealt (PDF stats table)
    constexpr int DAMAGE_MELEE_PLAYER   = 1;
    constexpr int DAMAGE_RANGED_PLAYER  = 1;
    constexpr int DAMAGE_DEFENSE_PLAYER = 0;

    constexpr int DAMAGE_EASY_ENEMY     = 1;
    constexpr int DAMAGE_MEDIUM_ENEMY   = 1;
    constexpr int DAMAGE_HARD_ENEMY     = 2;

    // Jump / Duck
    constexpr float JUMP_DURATION       = 0.70f;
    constexpr float JUMP_HEIGHT         = 45.0f;
    constexpr float DUCK_DURATION       = 0.70f;

    // Waves (US-39)
    constexpr int WAVE1_ENEMY_COUNT     = 3;
    constexpr int WAVE2_ENEMY_COUNT     = 5;
    constexpr int WAVE3_ENEMY_COUNT     = 10;
    constexpr float SPAWN_INTERVAL      = 2.8f;
    constexpr float WAVE_REST_PAUSE     = 3.5f;

    // Cutscene (US-06: 10-20 seconds)
    constexpr float CUTSCENE_DURATION   = 18.0f;
}
