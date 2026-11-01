#include "Menus.h"
#include "../core/Game.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include "TextRenderer.h"

Menus& Menus::instance() {
    static Menus m;
    return m;
}

void Menus::init() {
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;
    const float btnW = 340.0f;
    const float btnH = 54.0f;
    const float btnX = (W - btnW) * 0.5f;

    // Main Menu
    m_btnStart.setBounds(btnX, 290.0f, btnW, btnH);
    m_btnStart.setLabel("START GAME");

    m_btnSound.setBounds(btnX, 215.0f, btnW, btnH);
    m_btnSound.setLabel("SOUND SETTINGS");

    m_btnExit.setBounds(btnX, 140.0f, btnW, btnH);
    m_btnExit.setLabel("EXIT");

    // Sound Settings Menu
    m_btnVolUp.setBounds(btnX, 330.0f, btnW, btnH);
    m_btnVolUp.setLabel("VOLUME +");

    m_btnVolDown.setBounds(btnX, 255.0f, btnW, btnH);
    m_btnVolDown.setLabel("VOLUME -");

    m_btnMute.setBounds(btnX, 180.0f, btnW, btnH);
    m_btnMute.setLabel("TOGGLE MUTE");

    m_btnSoundBack.setBounds(btnX, 105.0f, btnW, btnH);
    m_btnSoundBack.setLabel("BACK TO MENU");

    // Pause Menu
    m_btnContinue.setBounds(btnX, 330.0f, btnW, btnH);
    m_btnContinue.setLabel("CONTINUE");

    m_btnPauseRestart.setBounds(btnX, 255.0f, btnW, btnH);
    m_btnPauseRestart.setLabel("RESTART");

    m_btnPauseMainMenu.setBounds(btnX, 180.0f, btnW, btnH);
    m_btnPauseMainMenu.setLabel("MAIN MENU");

    // End Menu
    m_btnEndRestart.setBounds(btnX, 240.0f, btnW, btnH);
    m_btnEndRestart.setLabel("RESTART GAME");

    m_btnEndMainMenu.setBounds(btnX, 165.0f, btnW, btnH);
    m_btnEndMainMenu.setLabel("MAIN MENU");
}

void Menus::updateMain(Game& game, glm::vec2 mousePos, bool leftClicked) {
    m_btnStart.update(mousePos, leftClicked);
    m_btnSound.update(mousePos, leftClicked);
    m_btnExit.update(mousePos, leftClicked);

    if (m_btnStart.wasClicked()) {
        AudioManager::instance().playSound(SoundID::PLAYER_PLACE);
        game.startCutscene();
    } else if (m_btnSound.wasClicked()) {
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
        game.setState(GameState::SOUND_SETTINGS);
    } else if (m_btnExit.wasClicked()) {
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
        game.quit();
    }
}

void Menus::drawMain() {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    // Background: Dark monastery courtyard night sky
    ren.drawRect(0, 0, W, H, glm::vec4(0.06f, 0.08f, 0.14f, 1.0f));

    // Distant mountain silhouettes
    for (int i = 0; i < 7; ++i) {
        float mx = i * 200.0f - 50.0f;
        ren.drawTri(glm::vec2(mx, 260.0f), glm::vec2(mx + 220.0f, 260.0f), glm::vec2(mx + 110.0f, 480.0f),
                    glm::vec4(0.10f, 0.13f, 0.22f, 1.0f));
    }
    // Cherry blossom trees & stone lanterns framing the menu
    ren.drawSprite("cherry_tree", 80.0f, 180.0f, 180.0f, 180.0f);
    ren.drawSprite("cherry_tree", W - 260.0f, 180.0f, 180.0f, 180.0f, true);
    ren.drawSprite("stone_lantern", 40.0f, 80.0f, 70.0f, 95.0f);
    ren.drawSprite("stone_lantern", W - 110.0f, 80.0f, 70.0f, 95.0f);

    // Characters framing the title (Sensei Wu & Kai)
    ren.drawSprite("wu", 200.0f, 120.0f, 140.0f, 160.0f);
    ren.drawSprite("kai", W - 340.0f, 120.0f, 140.0f, 180.0f);

    // Title banner with classic Pokemon / Game Boy double gold border
    float boxW = 860.0f, boxH = 150.0f;
    float boxX = (W - boxW) * 0.5f, boxY = 490.0f;
    ren.drawRect(boxX - 4, boxY - 4, boxW + 8, boxH + 8, glm::vec4(0.85f, 0.70f, 0.20f, 1.0f));
    ren.drawRect(boxX, boxY, boxW, boxH, glm::vec4(0.10f, 0.14f, 0.22f, 0.95f));
    ren.drawRectOutline(boxX + 4, boxY + 4, boxW - 8, boxH - 8, 2.0f, glm::vec4(0.35f, 0.45f, 0.65f, 0.8f));

    tr.drawTextCenteredWithShadow("NINJAGO: DEFEND THE MONASTERY", W * 0.5f, boxY + 80.0f, 3.8f,
                                  glm::vec4(1.0f, 0.88f, 0.30f, 1.0f), glm::vec4(0.02f, 0.02f, 0.05f, 1.0f));
    tr.drawTextCenteredWithShadow("SERPENTINE INVASION - 2D DEFENSE", W * 0.5f, boxY + 40.0f, 2.2f,
                                  glm::vec4(0.75f, 0.88f, 0.98f, 1.0f), glm::vec4(0.02f, 0.02f, 0.05f, 1.0f));

    // Draw buttons
    m_btnStart.draw();
    m_btnSound.draw();
    m_btnExit.draw();

    // Footer info
    tr.drawTextCentered("DMET 502 PROJECT 1 - GAME BOY PIXEL ART EDITION", W * 0.5f, 30.0f, 1.8f,
                        glm::vec4(0.50f, 0.60f, 0.75f, 0.8f));
}

void Menus::updateSound(Game& game, glm::vec2 mousePos, bool leftClicked) {
    m_btnVolUp.update(mousePos, leftClicked);
    m_btnVolDown.update(mousePos, leftClicked);
    m_btnMute.update(mousePos, leftClicked);
    m_btnSoundBack.update(mousePos, leftClicked);

    if (m_btnVolUp.wasClicked()) {
        AudioManager::instance().volumeUp();
    } else if (m_btnVolDown.wasClicked()) {
        AudioManager::instance().volumeDown();
    } else if (m_btnMute.wasClicked()) {
        AudioManager::instance().toggleMute();
    } else if (m_btnSoundBack.wasClicked()) {
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
        game.setState(GameState::MAIN_MENU);
    }
}

void Menus::drawSound() {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    ren.drawRect(0, 0, W, H, glm::vec4(0.07f, 0.09f, 0.15f, 1.0f));

    // Title box
    tr.drawTextCenteredWithShadow("SOUND & AUDIO SETTINGS", W * 0.5f, 540.0f, 4.0f,
                                  glm::vec4(1.0f, 0.88f, 0.30f, 1.0f));

    // Volume level bar (US-57)
    float vol = AudioManager::instance().getVolume();
    bool muted = AudioManager::instance().isMuted();

    std::string volStr = "VOLUME: " + (muted ? std::string("MUTED") : std::to_string((int)(vol * 100)) + "%");
    tr.drawTextCenteredWithShadow(volStr, W * 0.5f, 440.0f, 2.8f,
                                  muted ? glm::vec4(0.9f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.4f, 0.9f, 0.4f, 1.0f));

    // Visual volume meter
    float meterW = 340.0f, meterH = 24.0f;
    float meterX = (W - meterW) * 0.5f, meterY = 400.0f;
    ren.drawRect(meterX - 2, meterY - 2, meterW + 4, meterH + 4, glm::vec4(0.3f, 0.4f, 0.5f, 1.0f));
    ren.drawRect(meterX, meterY, meterW, meterH, glm::vec4(0.1f, 0.14f, 0.2f, 1.0f));
    if (!muted && vol > 0.01f) {
        ren.drawRect(meterX, meterY, meterW * vol, meterH, glm::vec4(0.2f, 0.85f, 0.4f, 1.0f));
    }

    m_btnVolUp.draw();
    m_btnVolDown.draw();
    m_btnMute.draw();
    m_btnSoundBack.draw();
}

void Menus::updatePause(Game& game, glm::vec2 mousePos, bool leftClicked) {
    m_btnContinue.update(mousePos, leftClicked);
    m_btnPauseRestart.update(mousePos, leftClicked);
    m_btnPauseMainMenu.update(mousePos, leftClicked);

    if (m_btnContinue.wasClicked()) {
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
        game.setState(GameState::PLAYING);
    } else if (m_btnPauseRestart.wasClicked()) {
        AudioManager::instance().playSound(SoundID::PLAYER_PLACE);
        game.reset();
        game.setState(GameState::PLAYING);
    } else if (m_btnPauseMainMenu.wasClicked()) {
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
        game.setState(GameState::MAIN_MENU);
    }
}

void Menus::drawPause() {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    // Translucent overlay freezing the scene behind (US-02)
    ren.drawRect(0, 0, W, H, glm::vec4(0.02f, 0.04f, 0.08f, 0.75f));

    // Pokemon style pause box
    float boxW = 460.0f, boxH = 400.0f;
    float boxX = (W - boxW) * 0.5f, boxY = 140.0f;
    ren.drawRect(boxX - 4, boxY - 4, boxW + 8, boxH + 8, glm::vec4(0.85f, 0.70f, 0.20f, 1.0f));
    ren.drawRect(boxX, boxY, boxW, boxH, glm::vec4(0.10f, 0.14f, 0.22f, 0.98f));
    ren.drawRectOutline(boxX + 4, boxY + 4, boxW - 8, boxH - 8, 2.0f, glm::vec4(0.35f, 0.45f, 0.65f, 0.8f));

    tr.drawTextCenteredWithShadow("GAME PAUSED", W * 0.5f, boxY + boxH - 55.0f, 4.0f,
                                  glm::vec4(1.0f, 0.88f, 0.30f, 1.0f));

    m_btnContinue.draw();
    m_btnPauseRestart.draw();
    m_btnPauseMainMenu.draw();
}

void Menus::updateEnd(Game& game, glm::vec2 mousePos, bool leftClicked, bool won) {
    (void)won;
    m_btnEndRestart.update(mousePos, leftClicked);
    m_btnEndMainMenu.update(mousePos, leftClicked);

    if (m_btnEndRestart.wasClicked()) {
        AudioManager::instance().playSound(SoundID::PLAYER_PLACE);
        game.reset();
        game.setState(GameState::PLAYING);
    } else if (m_btnEndMainMenu.wasClicked()) {
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
        game.setState(GameState::MAIN_MENU);
    }
}

void Menus::drawEnd(bool won) {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    // Translucent overlay (US-03)
    ren.drawRect(0, 0, W, H, glm::vec4(0.02f, 0.04f, 0.08f, 0.80f));

    float boxW = 580.0f, boxH = 430.0f;
    float boxX = (W - boxW) * 0.5f, boxY = 120.0f;

    glm::vec4 borderColor = won ? glm::vec4(0.85f, 0.75f, 0.20f, 1.0f) : glm::vec4(0.90f, 0.25f, 0.25f, 1.0f);
    ren.drawRect(boxX - 4, boxY - 4, boxW + 8, boxH + 8, borderColor);
    ren.drawRect(boxX, boxY, boxW, boxH, glm::vec4(0.10f, 0.14f, 0.22f, 0.98f));
    ren.drawRectOutline(boxX + 4, boxY + 4, boxW - 8, boxH - 8, 2.0f, glm::vec4(0.35f, 0.45f, 0.65f, 0.8f));

    if (won) {
        tr.drawTextCenteredWithShadow("VICTORY!", W * 0.5f, boxY + boxH - 60.0f, 5.0f,
                                      glm::vec4(1.0f, 0.88f, 0.25f, 1.0f));
        tr.drawTextCenteredWithShadow("MONASTERY DEFENDED!", W * 0.5f, boxY + boxH - 105.0f, 2.6f,
                                      glm::vec4(0.40f, 0.95f, 0.55f, 1.0f));
        tr.drawTextCenteredWithShadow("The Serpentine army was repelled!", W * 0.5f, boxY + boxH - 145.0f, 2.0f,
                                      glm::vec4(0.85f, 0.90f, 0.95f, 1.0f));
    } else {
        tr.drawTextCenteredWithShadow("DEFEAT!", W * 0.5f, boxY + boxH - 60.0f, 5.0f,
                                      glm::vec4(1.0f, 0.25f, 0.25f, 1.0f));
        tr.drawTextCenteredWithShadow("MONASTERY BREACHED!", W * 0.5f, boxY + boxH - 105.0f, 2.6f,
                                      glm::vec4(0.95f, 0.45f, 0.45f, 1.0f));
        tr.drawTextCenteredWithShadow("An enemy reached the monastery gate!", W * 0.5f, boxY + boxH - 145.0f, 2.0f,
                                      glm::vec4(0.85f, 0.90f, 0.95f, 1.0f));
    }

    m_btnEndRestart.draw();
    m_btnEndMainMenu.draw();
}
