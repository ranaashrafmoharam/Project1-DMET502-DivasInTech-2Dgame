#pragma once
// ====================================================================
// FILE:     src/core/Game.h
// OWNER:    Member 1
// TASKS:    M1-01, M1-03, M1-05, M1-11
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   The heart of the program. Owns the state machine, the main loop,
//   all entity lists and the reset logic. Every other system is
//   called from here.
//
// RESPONSIBILITIES:
//   - Run the loop: input -> update -> render, once per frame
//   - Switch states and keep track of the current state
//   - Hold lists of players, enemies, projectiles and collectibles
//   - Call each system's update/draw depending on the state
//   - Detect win/lose and switch to WON / LOST
//   - reset(): clear everything so Restart works from any state
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - run()
//   - setState(state)
//   - reset()
//   - update(dt)
//   - render()
//
// INTERACTS WITH: Input, Renderer, LaneGrid, WaveManager, CollisionSystem, Scene, HUD, Menus, Cutscene
// ====================================================================
