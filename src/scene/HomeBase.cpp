#include "HomeBase.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"

HomeBase& HomeBase::instance() {
    static HomeBase hb;
    return hb;
}

void HomeBase::draw() {
    Renderer& ren = Renderer::instance();
    // Draws the ornate monastery pagoda gate covering the 4 lanes
    float gateH = Constants::NUM_LANES * Constants::LANE_HEIGHT + 40.0f;
    ren.drawSprite("monastery_gate", 0.0f, Constants::FIELD_Y0 - 20.0f, Constants::HOME_BASE_WIDTH, gateH);

    // Glowing golden protective barrier line (the losing line!)
    ren.drawRect(Constants::FIELD_X0 - 3.0f, Constants::FIELD_Y0, 4.0f, Constants::NUM_LANES * Constants::LANE_HEIGHT,
                 glm::vec4(1.0f, 0.85f, 0.20f, 0.85f));
}
