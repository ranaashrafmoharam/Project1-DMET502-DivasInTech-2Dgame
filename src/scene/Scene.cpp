#include "Scene.h"
#include "HomeBase.h"
#include "EnvironmentElements.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"

Scene& Scene::instance() {
    static Scene s;
    return s;
}

void Scene::init() {}

void Scene::update(float dt) {
    EnvironmentElements::instance().update(dt);
}

void Scene::draw() {
    Renderer& ren = Renderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    // 1. Sky & Mountain Background
    ren.drawRect(0, 0, W, H, glm::vec4(0.09f, 0.11f, 0.18f, 1.0f));

    // Distant mountain ranges
    for (int i = 0; i < 7; ++i) {
        float mx = i * 210.0f - 50.0f;
        ren.drawTri(glm::vec2(mx, 530.0f), glm::vec2(mx + 240.0f, 530.0f), glm::vec2(mx + 120.0f, 680.0f),
                    glm::vec4(0.13f, 0.16f, 0.25f, 1.0f));
    }

    // Upper courtyard floor strip
    ren.drawRect(0, 540.0f, W, 120.0f, glm::vec4(0.18f, 0.20f, 0.26f, 1.0f));
    ren.drawRect(0, 540.0f, W, 3.0f, glm::vec4(0.28f, 0.32f, 0.42f, 1.0f));

    // 2. Lanes (US-16: Ground strips and separators make lane boundaries clear)
    const float laneFieldW = Constants::FIELD_X1 - Constants::FIELD_X0;
    for (int i = 0; i < Constants::NUM_LANES; ++i) {
        float y = Constants::laneY(i);

        // Alternating courtyard cobblestone tones
        glm::vec4 laneColor = (i % 2 == 0)
            ? glm::vec4(0.15f, 0.18f, 0.24f, 1.0f)
            : glm::vec4(0.17f, 0.20f, 0.27f, 1.0f);
        ren.drawRect(Constants::FIELD_X0, y, laneFieldW, Constants::LANE_HEIGHT, laneColor);

        // Cobblestone tile seam accents
        for (float sx = Constants::FIELD_X0 + 80.0f; sx < Constants::FIELD_X1; sx += 120.0f) {
            float seamOffset = (i % 2) * 40.0f;
            ren.drawRect(sx + seamOffset, y + 2.0f, 2.0f, Constants::LANE_HEIGHT - 4.0f, glm::vec4(0.10f, 0.12f, 0.16f, 0.40f));
        }

        // Wooden lane dividers
        ren.drawRect(Constants::FIELD_X0, y, laneFieldW, 3.0f, glm::vec4(0.08f, 0.10f, 0.14f, 1.0f));
    }

    // Top boundary line of lanes
    float topLaneY = Constants::FIELD_Y0 + Constants::NUM_LANES * Constants::LANE_HEIGHT;
    ren.drawRect(Constants::FIELD_X0, topLaneY - 3.0f, laneFieldW, 3.0f, glm::vec4(0.08f, 0.10f, 0.14f, 1.0f));

    // Right enemy spawn boundary marker
    ren.drawRect(Constants::FIELD_X1, Constants::FIELD_Y0, 4.0f, Constants::NUM_LANES * Constants::LANE_HEIGHT,
                 glm::vec4(0.35f, 0.15f, 0.15f, 0.6f));

    // 3. Environment Elements (US-52)
    EnvironmentElements::instance().draw();

    // 4. Home Base (US-12)
    HomeBase::instance().draw();
}
