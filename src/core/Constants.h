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
    constexpr float HUD_HEIGHT = 100.0f;
    constexpr float TOP_BAR_HEIGHT = 390.0f; // First battle lane begins below the scene backdrop
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
    constexpr float HOME_BASE_WIDTH = 252.0f;
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

    // Character-system aliases used by Member 2's models and combat code.
    constexpr float WORLD_WIDTH = static_cast<float>(SCREEN_WIDTH);
    constexpr float WORLD_HEIGHT = static_cast<float>(SCREEN_HEIGHT);
    constexpr float CHARACTER_WIDTH = 42.0f;
    constexpr float CHARACTER_HEIGHT = 74.0f;
    constexpr float PLAYER_MAX_X = static_cast<float>(SCREEN_WIDTH) - CHARACTER_WIDTH * 0.5f;
    constexpr float PLAYER_SPEED = SPEED_PLAYER_MELEE;
    constexpr float DEFENSE_SPEED = SPEED_PLAYER_DEFENSE;
    constexpr float ENEMY_SPEED = SPEED_ENEMY_EASY;
    constexpr float HARD_ENEMY_SPEED = SPEED_ENEMY_HARD;
    constexpr float MELEE_HEALTH = static_cast<float>(HP_PLAYER_MELEE);
    constexpr float RANGED_HEALTH = static_cast<float>(HP_PLAYER_RANGED);
    constexpr float DEFENSE_HEALTH = static_cast<float>(HP_PLAYER_DEFENSE);
    constexpr float ENEMY_EASY_HEALTH = static_cast<float>(HP_ENEMY_EASY);
    constexpr float ENEMY_MEDIUM_HEALTH = static_cast<float>(HP_ENEMY_MEDIUM);
    constexpr float ENEMY_HARD_HEALTH = static_cast<float>(HP_ENEMY_HARD);
    constexpr float PLAYER_DAMAGE = static_cast<float>(DMG_PLAYER_MELEE);
    constexpr float DEFENSE_DAMAGE = static_cast<float>(DMG_PLAYER_DEFENSE);
    constexpr float ENEMY_EASY_DAMAGE = static_cast<float>(DMG_ENEMY_EASY);
    constexpr float ENEMY_MEDIUM_DAMAGE = static_cast<float>(DMG_ENEMY_MEDIUM);
    constexpr float ENEMY_HARD_DAMAGE = static_cast<float>(DMG_ENEMY_HARD);
    constexpr float PLAYER_ATTACK_COOLDOWN = ATTACK_COOLDOWN_MELEE;
    constexpr float ENEMY_ATTACK_COOLDOWN = ATTACK_COOLDOWN_RANGED;
    constexpr float HARD_ENEMY_COOLDOWN = ATTACK_COOLDOWN_MELEE;
    constexpr float ATTACK_ANIM_TIME = 0.35f;
    constexpr float MELEE_RANGE = 62.0f;
    constexpr float RANGED_RANGE = RANGED_ATTACK_RANGE;
    constexpr float ENEMY_RANGED_RANGE = RANGED_ATTACK_RANGE;
    constexpr float ENEMY_MELEE_RANGE = 58.0f;
    constexpr float PROJECTILE_SPEED = SPEED_PROJECTILE;
    constexpr float JUMP_HEIGHT = 78.0f;
    constexpr float DUCK_SCALE_Y = 0.55f;
    constexpr int ORB_VALUE = COIN_DROP_VALUE;

    inline float laneCenterY(int lane) {
        return static_cast<float>(SCREEN_HEIGHT) - getLaneCenterY(lane);
    }

} // namespace Constants

namespace C = Constants;
