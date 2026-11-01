#pragma once
// ====================================================================
// FILE:     src/core/GameState.h
// Defines the list of game states used by the state machine.
// ====================================================================

enum class GameState {
    MAIN_MENU,
    SOUND_SETTINGS,
    CUTSCENE,
    PLAYING,
    PAUSED,
    WON,
    LOST
};
