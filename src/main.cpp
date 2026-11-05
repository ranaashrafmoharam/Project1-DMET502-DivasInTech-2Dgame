// ====================================================================
// FILE:     src/main.cpp
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Program entry point. Creates the Game object, runs it until the
//   window closes, then shuts everything down.
// ====================================================================

#include "core/Game.h"
#include <iostream>

int main() {
    Game game;

    if (!game.init()) {
        std::cerr << "[Main] Failed to initialize game!" << std::endl;
        return -1;
    }

    game.run();

    return 0;
}
