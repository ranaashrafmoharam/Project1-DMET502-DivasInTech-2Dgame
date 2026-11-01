#include "EnvironmentElements.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include <cmath>

EnvironmentElements& EnvironmentElements::instance() {
    static EnvironmentElements ee;
    return ee;
}

void EnvironmentElements::update(float dt) {
    m_animTime += dt;
}

void EnvironmentElements::draw() {
    Renderer& ren = Renderer::instance();
    const float W = Constants::WINDOW_WIDTH;

    // Subtle wind sway animation for cherry blossom trees and banners
    float sway = std::sin(m_animTime * 2.5f) * 0.04f;

    // 1. Japanese Cherry Blossom Trees (Element 1)
    ren.drawSprite("cherry_tree", 175.0f, 545.0f, 130.0f, 130.0f, false, sway);
    ren.drawSprite("cherry_tree", W - 220.0f, 545.0f, 130.0f, 130.0f, true, -sway);

    // 2. Stone Lanterns (Element 2)
    ren.drawSprite("stone_lantern", 320.0f, 550.0f, 45.0f, 65.0f);
    ren.drawSprite("stone_lantern", W - 310.0f, 550.0f, 45.0f, 65.0f);

    // 3. Wooden Training Dummy (Element 3)
    ren.drawSprite("training_dummy", 460.0f, 550.0f, 46.0f, 64.0f);

    // 4. Monastery Prayer Gong (Element 4)
    ren.drawSprite("prayer_gong", 620.0f, 550.0f, 64.0f, 64.0f);

    // 5. Dragon Clan Banners (Element 5)
    ren.drawSprite("dragon_banner", 780.0f, 550.0f, 40.0f, 75.0f, false, sway * 0.5f);
    ren.drawSprite("dragon_banner", 910.0f, 550.0f, 40.0f, 75.0f, false, -sway * 0.5f);
}
