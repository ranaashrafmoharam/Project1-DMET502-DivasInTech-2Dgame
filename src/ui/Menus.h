#pragma once
// ====================================================================
// FILE:     src/ui/Menus.h
// OWNER:    Member 4
// TASKS:    M4-02, M4-03, M4-04
// The Main, Pause, Win/Lose, and Sound Settings menus.
// ====================================================================

#include <glm/glm.hpp>
#include "Button.h"

class Game;

class Menus {
public:
    static Menus& instance();

    void init();

    // Updates & renders per state
    void updateMain(Game& game, glm::vec2 mousePos, bool leftClicked);
    void drawMain();

    void updateSound(Game& game, glm::vec2 mousePos, bool leftClicked);
    void drawSound();

    void updatePause(Game& game, glm::vec2 mousePos, bool leftClicked);
    void drawPause();

    void updateEnd(Game& game, glm::vec2 mousePos, bool leftClicked, bool won);
    void drawEnd(bool won);

private:
    Menus() = default;

    // Main Menu Buttons
    Button m_btnStart;
    Button m_btnSound;
    Button m_btnExit;

    // Sound Menu Buttons
    Button m_btnVolUp;
    Button m_btnVolDown;
    Button m_btnMute;
    Button m_btnSoundBack;

    // Pause Menu Buttons
    Button m_btnContinue;
    Button m_btnPauseRestart;
    Button m_btnPauseMainMenu;

    // End Menu Buttons
    Button m_btnEndRestart;
    Button m_btnEndMainMenu;
};
