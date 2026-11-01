#include "HUD.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "TextRenderer.h"
#include <cmath>

HUD& HUD::instance() {
    static HUD h;
    return h;
}

void HUD::update(float dt) {
    m_animTime += dt;
}

void HUD::draw(int currentWave, int totalWaves, int enemiesRemaining, const std::string& waveName) {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    // Top HUD Bar background banner
    float barH = 50.0f;
    float barY = H - barH;
    ren.drawRect(0, barY, W, barH, glm::vec4(0.08f, 0.11f, 0.18f, 0.95f));
    ren.drawRect(0, barY, W, 3.0f, glm::vec4(0.85f, 0.70f, 0.20f, 1.0f));

    // 1. Currency Display (US-17: Coin total shown at top of screen)
    // Golden Shard icon with subtle bob/shine
    float shardX = 28.0f;
    float shardY = barY + 9.0f;
    float shardBob = std::sin(m_animTime * 5.0f) * 2.0f;
    ren.drawSpriteCentered("shard", shardX + 16.0f, shardY + 16.0f + shardBob, 32.0f, 32.0f);

    std::string coinStr = "SHARDS: " + std::to_string(m_coins);
    tr.drawTextWithShadow(coinStr, shardX + 42.0f, barY + 15.0f, 2.5f,
                          glm::vec4(1.0f, 0.88f, 0.25f, 1.0f), glm::vec4(0.05f, 0.05f, 0.08f, 1.0f));

    // 2. Wave indicator & Enemies remaining (US-40)
    std::string waveStr = "WAVE " + std::to_string(currentWave) + "/" + std::to_string(totalWaves) + ": " + waveName;
    tr.drawTextCenteredWithShadow(waveStr, W * 0.5f, barY + 15.0f, 2.5f,
                                  glm::vec4(0.95f, 0.95f, 1.0f, 1.0f), glm::vec4(0.05f, 0.05f, 0.08f, 1.0f));

    std::string enemyStr = "ENEMIES: " + std::to_string(enemiesRemaining);
    tr.drawTextWithShadow(enemyStr, W - 230.0f, barY + 15.0f, 2.3f,
                          glm::vec4(0.95f, 0.45f, 0.45f, 1.0f), glm::vec4(0.05f, 0.05f, 0.08f, 1.0f));

    // Bottom Help & Control Info Banner
    float btmH = 28.0f;
    ren.drawRect(0, 0, W, btmH, glm::vec4(0.06f, 0.08f, 0.12f, 0.95f));
    ren.drawRect(0, btmH - 2.0f, W, 2.0f, glm::vec4(0.20f, 0.30f, 0.45f, 1.0f));

    tr.drawTextCentered("CLICK CARD -> SELECT LANE TO PLACE  |  W/UP: JUMP  |  S/DOWN: DUCK  |  ESC: PAUSE",
                        W * 0.5f, 7.0f, 1.6f, glm::vec4(0.70f, 0.80f, 0.92f, 0.9f));
}
