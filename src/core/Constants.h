#pragma once
// ====================================================================
// FILE:     src/core/Constants.h
// OWNER:    Member 1
// TASKS:    M1-02
// STATUS:   MINIMAL STUB IMPLEMENTED BY MEMBER 2 (agreed) - every value
//           needed by Member 2's players, enemies and projectiles is
//           here so nothing is hard-coded. Member 1 may extend it.
//
// PURPOSE:
//   Single source of truth for every shared number so nobody hard-
//   codes values in their own files.
//
// RESPONSIBILITIES:
//   - Window size and projection bounds
//   - Lane count (3+), lane height, lane y-positions
//   - Home-base x boundary, enemy spawn x, player spawn x
//   - Movement speeds (player, enemy, Hard enemy)
//   - Health, damage and cooldown values per unit type
//   - Card costs (Melee, Ranged = 2x, Defense = 3x) and starting coins
//   - Wave sizes (3 / 5 / 10) and spawn interval
//
// INTERACTS WITH: Used by every file
// ====================================================================

#include "Math.h"

namespace Constants {

    // ---------------------------------------------------- window / world
    constexpr int   WINDOW_WIDTH  = 1280;
    constexpr int   WINDOW_HEIGHT = 720;
    constexpr float WORLD_WIDTH   = (float)WINDOW_WIDTH;
    constexpr float WORLD_HEIGHT  = (float)WINDOW_HEIGHT;

    // ------------------------------------------------------------- lanes
    // 4 lanes, viewed from the side.  Row 0 is the bottom lane.
    constexpr int   LANE_COUNT       = 4;
    constexpr float PLAYFIELD_BOTTOM = 150.0f;
    constexpr float PLAYFIELD_TOP    = 650.0f;
    constexpr float LANE_HEIGHT      = (PLAYFIELD_TOP - PLAYFIELD_BOTTOM) / (float)LANE_COUNT;

    // Vertical centre of a lane (feet line for the units standing in it).
    inline float laneCenterY(int lane) {
        return PLAYFIELD_BOTTOM + LANE_HEIGHT * ((float)lane + 0.5f);
    }
    // Which lane a world-space y belongs to (-1 when outside the playfield).
    inline int laneIndexAt(float worldY) {
        if (worldY < PLAYFIELD_BOTTOM || worldY > PLAYFIELD_TOP) return -1;
        return (int)((worldY - PLAYFIELD_BOTTOM) / LANE_HEIGHT);
    }

    // --------------------------------------------------------- x boundary
    constexpr float HOME_BASE_X    = 120.0f;   // right edge of the monastery gate = losing line
    constexpr float PLAYER_SPAWN_X = 150.0f;   // players appear at the left end of their lane
    constexpr float ENEMY_SPAWN_X  = 1240.0f;  // enemies appear at the right edge
    constexpr float PLAYER_MAX_X   = 1000.0f;  // players stop advancing here so they stay on screen

    // ------------------------------------------------------------ speeds
    constexpr float PLAYER_SPEED      = 45.0f;   // Melee / Ranged advance to the right
    constexpr float DEFENSE_SPEED     = 0.0f;    // Defense never moves
    constexpr float ENEMY_SPEED       = 42.0f;   // Easy / Medium "normal speed"
    constexpr float HARD_ENEMY_SPEED  = 78.0f;   // Hard runs faster than the others

    // ------------------------------------------------ health and damage
    constexpr float MELEE_HEALTH        = 5.0f;
    constexpr float RANGED_HEALTH       = 5.0f;
    constexpr float DEFENSE_HEALTH      = 7.0f;   // absorbs 7 hits then is removed
    constexpr float ENEMY_EASY_HEALTH   = 5.0f;
    constexpr float ENEMY_MEDIUM_HEALTH = 5.0f;
    constexpr float ENEMY_HARD_HEALTH   = 7.0f;

    constexpr float PLAYER_DAMAGE   = 1.0f;   // every player except Defense deals 1
    constexpr float DEFENSE_DAMAGE  = 0.0f;   // Defense does not attack
    constexpr float ENEMY_EASY_DAMAGE   = 1.0f;
    constexpr float ENEMY_MEDIUM_DAMAGE = 1.0f;
    constexpr float ENEMY_HARD_DAMAGE   = 2.0f;

    // --------------------------------------------------------- cooldowns
    constexpr float PLAYER_ATTACK_COOLDOWN = 1.10f;
    constexpr float ENEMY_ATTACK_COOLDOWN  = 1.50f;
    constexpr float HARD_ENEMY_COOLDOWN    = 1.10f;   // fast brute swings often
    constexpr float ATTACK_ANIM_TIME       = 0.35f;   // how long the swing pose lasts

    // --------------------------------------------------------- movement
    constexpr float CHARACTER_WIDTH  = 42.0f;
    constexpr float CHARACTER_HEIGHT = 74.0f;

    constexpr float MELEE_RANGE   = 62.0f;    // Melee hits when this close
    constexpr float RANGED_RANGE  = 620.0f;   // Ranged fires across most of the lane
    constexpr float ENEMY_RANGED_RANGE = 620.0f;
    constexpr float ENEMY_MELEE_RANGE  = 58.0f;

    // ------------------------------------------------------- projectile
    constexpr float PROJECTILE_SPEED = 460.0f;
    constexpr float PROJECTILE_WIDTH = 18.0f;

    // -------------------------------------------------------------- jump
    constexpr float JUMP_HEIGHT   = 78.0f;   // visible arc height (pixels)
    constexpr float JUMP_DURATION = 0.70f;   // whole up-and-down arc

    // -------------------------------------------------------------- duck
    constexpr float DUCK_DURATION = 0.70f;   // how long the duck pose is held
    constexpr float DUCK_SCALE_Y  = 0.55f;   // squash factor of the upper body

    // ------------------------------------------ costs, coins, collectible
    constexpr int MELEE_COST      = 10;   // starter player
    constexpr int RANGED_COST     = MELEE_COST * 2;   // unlocked at 2x
    constexpr int DEFENSE_COST    = MELEE_COST * 3;   // unlocked at 3x
    constexpr int STARTING_COINS  = 30;   // enough for exactly three Melee players
    constexpr int ORB_VALUE       = 5;    // coins gained from one Spinjitzu Energy Orb

    // -------------------------------------------------------------- waves
    constexpr int WAVE1_SIZE = 3;    // Easy only
    constexpr int WAVE2_SIZE = 5;    // Easy + Medium
    constexpr int WAVE3_SIZE = 10;   // Easy + Medium + Hard
    constexpr float WAVE_SPAWN_INTERVAL = 2.2f;

    // ------------------------------------------------------- health bar
    constexpr float HEALTH_BAR_WIDTH  = 40.0f;
    constexpr float HEALTH_BAR_HEIGHT = 6.0f;
    constexpr float HEALTH_BAR_OFFSET = 10.0f;  // above the unit's head

} // namespace Constants

// Convenience so files do not have to write Constants:: everywhere.
namespace C = Constants;
