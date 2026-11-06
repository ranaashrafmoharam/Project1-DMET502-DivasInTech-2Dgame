#pragma once
// ====================================================================
// FILE:     src/core/GameState.h
// OWNER:    Member 1
// TASKS:    M1-03
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Defines the list of game states used by the state machine.
//
// RESPONSIBILITIES:
//   - MAIN_MENU, CUTSCENE, PLAYING, PAUSED, WON, LOST
//
// INTERACTS WITH: core/Game, ui/Menus, cutscene/Cutscene
// ====================================================================

enum class GameState {
    MAIN_MENU,
    CUTSCENE,
    PLAYING,
    PAUSED,
    WON,
    LOST
};
