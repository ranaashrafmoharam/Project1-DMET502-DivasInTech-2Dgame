#include "Menus.h"
#include "../core/Game.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include "TextRenderer.h"
#include <GLFW/glfw3.h>
#include <cmath>

namespace {

void drawMainMenuBackground(Renderer& ren, float W, float H) {
    float time = static_cast<float>(glfwGetTime());

    // Layered dusk sky, using many small primitive bands instead of an image.
    for (int i = 0; i < 48; ++i) {
        float y = i * (H / 48.0f);
        float t = i / 47.0f;
        ren.drawRect(0.0f, y, W, H / 48.0f + 1.0f,
                     glm::vec4(0.018f + 0.055f * t,
                               0.025f + 0.075f * t,
                               0.080f + 0.12f * t, 1.0f));
    }

    // Large moon and soft moonlit glow.
    ren.drawCircle(1060.0f, 625.0f, 66.0f, glm::vec4(0.62f, 0.73f, 0.90f, 0.10f));
    ren.drawCircle(1060.0f, 625.0f, 42.0f, glm::vec4(0.96f, 0.96f, 0.82f, 1.0f));
    ren.drawCircle(1046.0f, 638.0f, 13.0f, glm::vec4(0.82f, 0.84f, 0.74f, 0.32f));
    ren.drawCircle(1073.0f, 612.0f, 9.0f, glm::vec4(0.82f, 0.84f, 0.74f, 0.28f));

    // Wide layered night clouds drifting in the moonlight.
    for (int i = 0; i < 7; ++i) {
        float cx = std::fmod(55.0f + i * 225.0f + time * (5.0f + i % 3), W + 180.0f) - 90.0f;
        float cy = 565.0f + (i % 3) * 48.0f;
        ren.drawCircle(cx, cy, 30.0f, glm::vec4(0.33f, 0.43f, 0.60f, 0.42f));
        ren.drawCircle(cx + 38.0f, cy - 5.0f, 38.0f, glm::vec4(0.42f, 0.51f, 0.67f, 0.40f));
        ren.drawCircle(cx + 82.0f, cy + 2.0f, 27.0f, glm::vec4(0.29f, 0.39f, 0.56f, 0.38f));
        ren.drawRect(cx - 8.0f, cy - 24.0f, 105.0f, 28.0f,
                     glm::vec4(0.29f, 0.39f, 0.56f, 0.30f));
    }

    // Distant mountain silhouettes with snow and foothills.
    for (int i = -1; i < 9; ++i) {
        float x = i * 190.0f;
        ren.drawTri(glm::vec2(x, 390.0f),
                    glm::vec2(x + 220.0f, 390.0f),
                    glm::vec2(x + 105.0f, 590.0f - (i % 2) * 55.0f),
                    glm::vec4(0.19f, 0.22f, 0.31f, 1.0f));
        ren.drawTri(glm::vec2(x + 105.0f, 590.0f - (i % 2) * 55.0f),
                    glm::vec2(x + 128.0f, 552.0f - (i % 2) * 55.0f),
                    glm::vec2(x + 76.0f, 545.0f - (i % 2) * 55.0f),
                    glm::vec4(0.58f, 0.62f, 0.69f, 0.78f));
    }

    // Moonlit monastery garden: grass beds, gravel paths and stone edging.
    ren.drawRect(0.0f, 0.0f, W, 250.0f, glm::vec4(0.08f, 0.18f, 0.12f, 1.0f));
    ren.drawRect(0.0f, 0.0f, W, 105.0f, glm::vec4(0.10f, 0.23f, 0.14f, 1.0f));
    ren.drawRect(0.0f, 232.0f, W, 18.0f, glm::vec4(0.31f, 0.28f, 0.22f, 1.0f));
    ren.drawRect(0.0f, 248.0f, W, 4.0f, glm::vec4(0.55f, 0.52f, 0.43f, 1.0f));

    auto drawFlowerBed = [&](float x, float width) {
        ren.drawRect(x, 100.0f, width, 118.0f, glm::vec4(0.09f, 0.18f, 0.11f, 1.0f));
        ren.drawRect(x, 100.0f, width, 8.0f, glm::vec4(0.34f, 0.31f, 0.25f, 1.0f));
        for (int i = 0; i < static_cast<int>(width / 34.0f); ++i) {
            float sx = x + 18.0f + i * 34.0f;
            float sy = 122.0f + (i % 3) * 28.0f;
            ren.drawRect(sx, sy, 4.0f, 18.0f, glm::vec4(0.14f, 0.39f, 0.18f, 1.0f));
            ren.drawCircle(sx + 2.0f, sy + 20.0f, 8.0f,
                           glm::vec4(0.82f, 0.46f, 0.64f, 1.0f));
            ren.drawCircle(sx + 2.0f, sy + 20.0f, 3.0f,
                           glm::vec4(0.96f, 0.82f, 0.35f, 1.0f));
        }
    };
    drawFlowerBed(0.0f, 270.0f);
    drawFlowerBed(W - 270.0f, 270.0f);

    // Central gravel walkway and irregular stepping stones.
    float cx = W * 0.5f;
    ren.drawRect(cx - 85.0f, 0.0f, 170.0f, 220.0f, glm::vec4(0.43f, 0.40f, 0.34f, 1.0f));
    for (int i = 0; i < 16; ++i) {
        float px = cx - 68.0f + (i % 3) * 31.0f;
        float py = 12.0f + i * 13.0f;
        ren.drawRect(px, py, 18.0f, 7.0f, glm::vec4(0.58f, 0.55f, 0.48f, 0.78f));
    }

    // Rounded evergreen shrubs separating the garden beds from the path.
    for (int i = 0; i < 16; ++i) {
        float sx = 18.0f + i * 78.0f;
        if (std::abs(sx - cx) < 105.0f || std::abs(sx + 18.0f - cx) < 105.0f)
            continue;
        ren.drawCircle(sx, 208.0f, 20.0f, glm::vec4(0.08f, 0.29f, 0.16f, 1.0f));
        ren.drawCircle(sx + 18.0f, 212.0f, 16.0f, glm::vec4(0.10f, 0.36f, 0.19f, 1.0f));
    }
    ren.drawRect(cx - 85.0f, 205.0f, 170.0f, 8.0f,
                 glm::vec4(0.18f, 0.14f, 0.12f, 1.0f));

    // Vermilion torii gate modeled from the supplied reference.
    cx = W * 0.5f;
    ren.drawRect(cx - 230.0f, 238.0f, 460.0f, 18.0f, glm::vec4(0.32f, 0.16f, 0.10f, 1.0f));
    ren.drawRect(cx - 185.0f, 250.0f, 42.0f, 270.0f, glm::vec4(0.88f, 0.19f, 0.08f, 1.0f));
    ren.drawRect(cx + 143.0f, 250.0f, 42.0f, 270.0f, glm::vec4(0.88f, 0.19f, 0.08f, 1.0f));
    ren.drawRect(cx - 198.0f, 250.0f, 68.0f, 28.0f, glm::vec4(0.12f, 0.10f, 0.08f, 1.0f));
    ren.drawRect(cx + 130.0f, 250.0f, 68.0f, 28.0f, glm::vec4(0.12f, 0.10f, 0.08f, 1.0f));
    ren.drawRect(cx - 200.0f, 485.0f, 400.0f, 35.0f, glm::vec4(0.88f, 0.18f, 0.07f, 1.0f));
    ren.drawRect(cx - 218.0f, 514.0f, 436.0f, 32.0f, glm::vec4(0.68f, 0.10f, 0.045f, 1.0f));
    ren.drawTri(glm::vec2(cx - 252.0f, 545.0f), glm::vec2(cx + 252.0f, 545.0f),
                glm::vec2(cx + 218.0f, 520.0f), glm::vec4(0.08f, 0.075f, 0.065f, 1.0f));
    ren.drawTri(glm::vec2(cx - 252.0f, 545.0f), glm::vec2(cx - 218.0f, 520.0f),
                glm::vec2(cx + 218.0f, 520.0f), glm::vec4(0.08f, 0.075f, 0.065f, 1.0f));
    ren.drawRect(cx - 238.0f, 535.0f, 476.0f, 18.0f, glm::vec4(0.10f, 0.085f, 0.07f, 1.0f));
    ren.drawRect(cx - 175.0f, 400.0f, 350.0f, 28.0f, glm::vec4(0.78f, 0.13f, 0.06f, 1.0f));
    ren.drawRect(cx - 158.0f, 426.0f, 316.0f, 13.0f, glm::vec4(0.68f, 0.08f, 0.04f, 1.0f));
    ren.drawRect(cx - 148.0f, 439.0f, 296.0f, 46.0f, glm::vec4(0.20f, 0.075f, 0.055f, 1.0f));
    ren.drawRect(cx - 140.0f, 445.0f, 280.0f, 34.0f, glm::vec4(0.90f, 0.22f, 0.09f, 1.0f));
    ren.drawRect(cx - 8.0f, 439.0f, 16.0f, 46.0f, glm::vec4(0.12f, 0.09f, 0.07f, 1.0f));
    for (int i = -2; i <= 2; ++i) {
        float bx = cx + i * 54.0f;
        ren.drawRect(bx - 3.0f, 442.0f, 6.0f, 40.0f, glm::vec4(0.36f, 0.07f, 0.04f, 1.0f));
        ren.drawCircle(bx, 462.0f, 8.0f, glm::vec4(0.78f, 0.55f, 0.18f, 1.0f));
    }
    ren.drawRect(cx - 72.0f, 474.0f, 144.0f, 11.0f, glm::vec4(0.10f, 0.085f, 0.065f, 1.0f));
    ren.drawRect(cx - 58.0f, 484.0f, 116.0f, 18.0f, glm::vec4(0.82f, 0.58f, 0.20f, 1.0f));
    ren.drawRect(cx - 48.0f, 490.0f, 96.0f, 7.0f, glm::vec4(0.12f, 0.09f, 0.07f, 1.0f));

    // Sakura trees, trunks, branches and layered blossom clusters.
    auto drawTree = [&](float baseX, bool flip) {
        float direction = flip ? -1.0f : 1.0f;
        float wind = std::sin(time * 1.15f + baseX * 0.02f);
        float sway = wind * 18.0f;
        float canopySway = wind * 26.0f;

        float trunkSway = sway * 0.14f;
        ren.drawRect(baseX - 18.0f + trunkSway, 250.0f, 36.0f, 225.0f,
                     glm::vec4(0.32f, 0.18f, 0.11f, 1.0f));
        ren.drawRect(baseX - 18.0f + trunkSway, 250.0f, 8.0f, 225.0f,
                     glm::vec4(0.44f, 0.26f, 0.16f, 1.0f));
        ren.drawRect(baseX - 5.0f + trunkSway, 475.0f, 10.0f, 38.0f,
                     glm::vec4(0.32f, 0.18f, 0.11f, 1.0f));

        auto drawPetalCluster = [&](float cx, float cy, float r, float amount) {
            float sx = cx + canopySway * amount;
            sx = 2.0f * baseX - sx;
            cy = 925.0f - cy;
            ren.drawCircle(sx, cy, r * 1.28f, glm::vec4(0.69f, 0.28f, 0.46f, 1.0f));
            ren.drawCircle(sx - r * 0.42f, cy + r * 0.08f, r * 0.82f,
                           glm::vec4(0.88f, 0.43f, 0.62f, 1.0f));
            ren.drawCircle(sx + r * 0.40f, cy + r * 0.02f, r * 0.78f,
                           glm::vec4(0.82f, 0.36f, 0.57f, 1.0f));
            ren.drawCircle(sx - r * 0.12f, cy - r * 0.38f, r * 0.86f,
                           glm::vec4(1.0f, 0.66f, 0.78f, 1.0f));
            ren.drawCircle(sx - r * 0.58f, cy - r * 0.30f, r * 0.52f,
                           glm::vec4(1.0f, 0.77f, 0.86f, 1.0f));
            ren.drawCircle(sx + r * 0.52f, cy - r * 0.34f, r * 0.54f,
                           glm::vec4(0.96f, 0.58f, 0.74f, 1.0f));
            ren.drawCircle(sx, cy - r * 0.72f, r * 0.46f,
                           glm::vec4(1.0f, 0.83f, 0.90f, 1.0f));
            for (int blossom = 0; blossom < 7; ++blossom) {
                float angle = blossom * 1.57f + r * 0.03f;
                float bx = sx + std::cos(angle) * r * (0.42f + (blossom % 2) * 0.12f);
                float by = cy + std::sin(angle) * r * (0.34f + (blossom % 3) * 0.08f);
                ren.drawCircle(bx, by, r * 0.18f,
                               glm::vec4(1.0f, 0.88f, 0.92f, 0.90f));
            }
        };
        drawPetalCluster(baseX - direction * 58.0f, 485.0f, 31.0f, 0.70f);
        drawPetalCluster(baseX + direction * 58.0f, 473.0f, 34.0f, 1.00f);
        drawPetalCluster(baseX - direction * 18.0f, 447.0f, 36.0f, 0.85f);
        drawPetalCluster(baseX + direction * 26.0f, 439.0f, 32.0f, 0.95f);
        drawPetalCluster(baseX, 473.0f, 38.0f, 0.80f);
    };
    drawTree(230.0f, false);
    drawTree(W - 230.0f, true);
    drawTree(75.0f, false);
    drawTree(W - 75.0f, true);

    // Falling blossom petals drift in the wind.
    for (int i = 0; i < 90; ++i) {
        float fallSpeed = 22.0f + (i % 7) * 5.0f;
        float y = H - std::fmod(time * fallSpeed + i * 37.0f, H + 120.0f);
        float x = std::fmod(i * 137.0f + time * (20.0f + (i % 4) * 4.0f)
                            + std::sin(time * 1.7f + i * 0.8f) * 38.0f, W);
        float petalW = 4.0f + (i % 3);
        ren.drawRect(x, y, petalW, petalW * 0.48f,
                     glm::vec4(1.0f, 0.62f + (i % 5) * 0.025f, 0.72f, 0.82f));
    }

    // Torches with layered glow, animated flame and sparks.
    auto drawTorch = [&](float x, float y) {
        float flicker = 0.82f + 0.18f * std::sin(time * 8.0f + x);
        ren.drawCircle(x, y + 76.0f, 54.0f * flicker, glm::vec4(1.0f, 0.48f, 0.12f, 0.10f));
        ren.drawCircle(x, y + 76.0f, 34.0f * flicker, glm::vec4(1.0f, 0.62f, 0.16f, 0.16f));
        ren.drawRect(x - 5.0f, y, 10.0f, 82.0f, glm::vec4(0.24f, 0.12f, 0.07f, 1.0f));
        ren.drawRect(x - 13.0f, y + 72.0f, 26.0f, 8.0f, glm::vec4(0.13f, 0.12f, 0.12f, 1.0f));
        ren.drawTri(glm::vec2(x - 11.0f, y + 78.0f), glm::vec2(x + 11.0f, y + 78.0f),
                    glm::vec2(x + std::sin(time * 9.0f) * 2.0f, y + 122.0f),
                    glm::vec4(1.0f, 0.36f, 0.05f, 0.95f));
        ren.drawTri(glm::vec2(x - 7.0f, y + 80.0f), glm::vec2(x + 7.0f, y + 80.0f),
                    glm::vec2(x, y + 110.0f + (flicker - 0.82f) * 12.0f),
                    glm::vec4(1.0f, 0.88f, 0.22f, 1.0f));
        for (int i = 0; i < 3; ++i) {
            float sparkY = y + 105.0f + std::fmod(time * (18.0f + i * 4.0f) + i * 18.0f, 38.0f);
            ren.drawCircle(x + std::sin(time * 5.0f + i) * 9.0f, sparkY, 2.0f,
                           glm::vec4(1.0f, 0.58f, 0.12f, 0.85f));
        }
    };
    drawTorch(355.0f, 250.0f);
    drawTorch(W - 355.0f, 250.0f);
}

} // namespace

Menus& Menus::instance() {
    static Menus m;
    return m;
}

void Menus::init() {
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;
    const float btnW = 400.0f;
    const float btnH = 54.0f;
    const float btnX = (W - btnW) * 0.5f;
    const float mainBtnW = 460.0f;
    const float mainBtnX = (W - mainBtnW) * 0.5f;

    // Main Menu
    m_btnStart.setBounds(mainBtnX, 290.0f, mainBtnW, btnH);
    m_btnStart.setLabel("START GAME");

    m_btnSound.setBounds(mainBtnX, 215.0f, mainBtnW, btnH);
    m_btnSound.setLabel("SOUND SETTINGS");

    m_btnExit.setBounds(mainBtnX, 140.0f, mainBtnW, btnH);
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

    drawMainMenuBackground(ren, W, H);

    const float frameW = 1100.0f;
    const float frameH = 220.0f;
    const float frameX = (W - frameW) * 0.5f;
    const float frameY = 475.0f;
    tr.drawTextCenteredWithShadow("NINJAGO: DEFEND THE MONASTERY", W * 0.5f, frameY + 112.0f, 3.7f,
                                  glm::vec4(1.0f, 0.88f, 0.30f, 1.0f),
                                  glm::vec4(0.02f, 0.02f, 0.05f, 1.0f));

    // Draw buttons
    m_btnStart.draw();
    m_btnSound.draw();
    m_btnExit.draw();

    // Footer info
    tr.drawTextCentered("DMET502 Project_1  2D_Graphics", W * 0.5f, 44.0f, 1.8f,
                        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    tr.drawTextCentered("made by rana, jana, haya and ziad", W * 0.5f, 24.0f, 1.8f,
                        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
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

    ren.drawSprite("settings_frame", (W - 560.0f) * 0.5f, 50.0f, 560.0f, 470.0f);

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

    float boxW = 760.0f, boxH = 540.0f;
    float boxX = (W - boxW) * 0.5f, boxY = 65.0f;

    glm::vec4 borderColor = won ? glm::vec4(0.20f, 0.90f, 0.35f, 1.0f) : glm::vec4(0.95f, 0.20f, 0.20f, 1.0f);
    ren.drawRect(boxX - 4, boxY - 4, boxW + 8, boxH + 8, borderColor);
    ren.drawRect(boxX, boxY, boxW, boxH, glm::vec4(0.10f, 0.14f, 0.22f, 0.98f));
    ren.drawRectOutline(boxX + 4, boxY + 4, boxW - 8, boxH - 8, 2.0f, glm::vec4(0.35f, 0.45f, 0.65f, 0.8f));

    if (won) {
        tr.drawTextCenteredWithShadow("VICTORY!", W * 0.5f, boxY + boxH - 90.0f, 6.0f,
                                      glm::vec4(1.0f, 0.88f, 0.25f, 1.0f));
        tr.drawTextCenteredWithShadow("MONASTERY DEFENDED!", W * 0.5f, boxY + boxH - 205.0f, 3.6f,
                                      glm::vec4(0.40f, 0.95f, 0.55f, 1.0f));
        tr.drawTextCenteredWithShadow("The Serpentine army was repelled!", W * 0.5f, boxY + boxH - 265.0f, 2.4f,
                                      glm::vec4(0.85f, 0.90f, 0.95f, 1.0f));
    } else {
        tr.drawTextCenteredWithShadow("DEFEAT!", W * 0.5f, boxY + boxH - 90.0f, 6.0f,
                                      glm::vec4(1.0f, 0.25f, 0.25f, 1.0f));
        tr.drawTextCenteredWithShadow("MONASTERY BREACHED!", W * 0.5f, boxY + boxH - 205.0f, 3.6f,
                                      glm::vec4(0.95f, 0.45f, 0.45f, 1.0f));
        tr.drawTextCenteredWithShadow("An enemy reached the monastery gate!", W * 0.5f, boxY + boxH - 265.0f, 2.4f,
                                      glm::vec4(0.85f, 0.90f, 0.95f, 1.0f));
    }

    glm::vec4 endAccent = won ? glm::vec4(0.20f, 0.90f, 0.35f, 1.0f)
                              : glm::vec4(0.95f, 0.20f, 0.20f, 1.0f);
    const float endButtonW = 420.0f;
    const float endButtonX = boxX + (boxW - endButtonW) * 0.5f;
    m_btnEndRestart.setBounds(endButtonX, 205.0f, endButtonW, 62.0f);
    m_btnEndMainMenu.setBounds(endButtonX, 125.0f, endButtonW, 62.0f);
    m_btnEndRestart.setAccentColor(endAccent);
    m_btnEndMainMenu.setAccentColor(endAccent);
    m_btnEndRestart.draw();
    m_btnEndMainMenu.draw();
}
