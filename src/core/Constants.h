#pragma once
// ====================================================================
// FILE:     src/core/Constants.h
// OWNER:    Member 1
// TASKS:    M1-02
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Single source of truth for every shared number so nobody hard-
//   codes values in their own files.
// ====================================================================

#include <vector>
#include <string>

namespace Constants {

    // ----------------------------------------------------------------
    // Window & Coordinate System
    // Top-left is (0, 0), +X goes right, +Y goes down
    // ----------------------------------------------------------------
    constexpr int SCREEN_WIDTH  = 1280;
    constexpr int SCREEN_HEIGHT = 720;
    const std::string WINDOW_TITLE = "Ninjago: 2D Tower Defense (DMET 502)";

    // ----------------------------------------------------------------
    // Lane Configuration
    // 3 distinct lanes stacked horizontally
    // ----------------------------------------------------------------
    constexpr int LANE_COUNT = 3;
    constexpr float TOP_BAR_HEIGHT = 100.0f; // UI / HUD area at top
    constexpr float PLAYFIELD_HEIGHT = SCREEN_HEIGHT - TOP_BAR_HEIGHT; // 620 px
    constexpr float LANE_HEIGHT = PLAYFIELD_HEIGHT / LANE_COUNT;       // ~206.67 px

    // Y-center coordinate for each lane (0, 1, 2)
    inline float getLaneCenterY(int lane) {
        if (lane < 0) lane = 0;
        if (lane >= LANE_COUNT) lane = LANE_COUNT - 1;
        return TOP_BAR_HEIGHT + (lane + 0.5f) * LANE_HEIGHT;
    }

    // Y-top boundary for each lane
    inline float getLaneTopY(int lane) {
        return TOP_BAR_HEIGHT + lane * LANE_HEIGHT;
    }

    // ----------------------------------------------------------------
    // Boundaries & X-Coordinates
    // ----------------------------------------------------------------
    constexpr float HOME_BASE_WIDTH = 180.0f;
    constexpr float HOME_BASE_X     = HOME_BASE_WIDTH; // Right edge of HomeBase (losing line)
    constexpr float PLAYER_SPAWN_X  = HOME_BASE_X + 20.0f; // 200.0f
    constexpr float ENEMY_SPAWN_X   = 1220.0f;             // Near rightmost edge
    constexpr float RIGHT_EDGE_X    = static_cast<float>(SCREEN_WIDTH);

    // ----------------------------------------------------------------
    // Unit Sizing & Hitbox Dimensions
    // ----------------------------------------------------------------
    constexpr float UNIT_WIDTH  = 80.0f;
    constexpr float UNIT_HEIGHT = 110.0f;
    constexpr float PROJECTILE_WIDTH  = 30.0f;
    constexpr float PROJECTILE_HEIGHT = 15.0f;
    constexpr float COLLECTIBLE_SIZE  = 40.0f;

    // ----------------------------------------------------------------
    // Movement Speeds (pixels per second)
    // ----------------------------------------------------------------
    constexpr float SPEED_PLAYER_MELEE   = 65.0f;
    constexpr float SPEED_PLAYER_RANGED  = 55.0f;
    constexpr float SPEED_PLAYER_DEFENSE = 0.0f; // Stationary

    constexpr float SPEED_ENEMY_EASY   = 45.0f;
    constexpr float SPEED_ENEMY_MEDIUM = 45.0f;
    constexpr float SPEED_ENEMY_HARD   = 85.0f; // Runs faster!

    constexpr float SPEED_PROJECTILE   = 280.0f;

    // ----------------------------------------------------------------
    // Health / Durability
    // ----------------------------------------------------------------
    constexpr int HP_PLAYER_MELEE   = 5;
    constexpr int HP_PLAYER_RANGED  = 5;
    constexpr int HP_PLAYER_DEFENSE = 7; // Absorbs 7 hits

    constexpr int HP_ENEMY_EASY   = 5;
    constexpr int HP_ENEMY_MEDIUM = 5;
    constexpr int HP_ENEMY_HARD   = 7;

    // ----------------------------------------------------------------
    // Damage Values
    // ----------------------------------------------------------------
    constexpr int DMG_PLAYER_MELEE   = 1;
    constexpr int DMG_PLAYER_RANGED  = 1;
    constexpr int DMG_PLAYER_DEFENSE = 0;

    constexpr int DMG_ENEMY_EASY   = 1;
    constexpr int DMG_ENEMY_MEDIUM = 1;
    constexpr int DMG_ENEMY_HARD   = 2; // Hard enemy deals 2 damage

    // ----------------------------------------------------------------
    // Attack Timers & Cooldowns (seconds)
    // ----------------------------------------------------------------
    constexpr float ATTACK_COOLDOWN_MELEE = 1.0f;
    constexpr float ATTACK_COOLDOWN_RANGED = 1.5f;
    constexpr float RANGED_ATTACK_RANGE = 450.0f; // Range for Ranged Player / Enemy to start shooting

    // Jump & Duck parameters
    constexpr float JUMP_DURATION = 0.6f;
    constexpr float DUCK_DURATION = 0.6f;
    constexpr float JUMP_HEIGHT_OFFSET = -70.0f; // Moves upwards in -Y

    // ----------------------------------------------------------------
    // Economy & Card Costs
    // ----------------------------------------------------------------
    constexpr int STARTING_COINS = 30; // Buys at least 3 Melee players
    constexpr int COST_MELEE   = 10;
    constexpr int COST_RANGED  = 20; // 2x Melee
    constexpr int COST_DEFENSE = 30; // 3x Melee
    constexpr int COIN_DROP_VALUE = 10;

    // ----------------------------------------------------------------
    // Waves Configuration
    // ----------------------------------------------------------------
    constexpr int WAVE_1_COUNT = 3;  // Wave 1: at least 3 Easy
    constexpr int WAVE_2_COUNT = 5;  // Wave 2: at least 5 Easy/Medium
    constexpr int WAVE_3_COUNT = 10; // Wave 3: at least 10 mixed
    constexpr float ENEMY_SPAWN_INTERVAL = 3.2f; // Seconds between spawns

} // namespace Constants
