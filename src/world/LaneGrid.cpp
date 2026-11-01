#include "LaneGrid.h"
#include "../core/Renderer.h"
#include "../audio/AudioManager.h"

LaneGrid& LaneGrid::instance() {
    static LaneGrid lg;
    return lg;
}

void LaneGrid::init() {
    reset();
}

void LaneGrid::reset() {
    m_hoveredLane = -1;
    for (int i = 0; i < Constants::NUM_LANES; ++i) {
        m_selected[i] = false;
    }
}

int LaneGrid::laneAt(glm::vec2 p) const {
    if (p.x < Constants::FIELD_X0 || p.x > Constants::FIELD_X1) return -1;
    float topY = Constants::FIELD_Y0 + Constants::NUM_LANES * Constants::LANE_HEIGHT;
    if (p.y < Constants::FIELD_Y0 || p.y >= topY) return -1;

    int fromBottom = static_cast<int>((p.y - Constants::FIELD_Y0) / Constants::LANE_HEIGHT);
    return Constants::NUM_LANES - 1 - fromBottom; // 0 = top lane
}

bool LaneGrid::isLaneSelected(int lane) const {
    if (lane >= 0 && lane < Constants::NUM_LANES) {
        return m_selected[lane];
    }
    return false;
}

void LaneGrid::toggleLane(int lane) {
    if (lane >= 0 && lane < Constants::NUM_LANES) {
        m_selected[lane] = !m_selected[lane];
        AudioManager::instance().playSound(SoundID::LANE_SELECT);
    }
}

void LaneGrid::selectSingleLane(int lane) {
    clearSelection();
    if (lane >= 0 && lane < Constants::NUM_LANES) {
        m_selected[lane] = true;
    }
}

void LaneGrid::clearSelection() {
    for (int i = 0; i < Constants::NUM_LANES; ++i) {
        m_selected[i] = false;
    }
}

std::vector<int> LaneGrid::getSelectedLanes() const {
    std::vector<int> sel;
    for (int i = 0; i < Constants::NUM_LANES; ++i) {
        if (m_selected[i]) sel.push_back(i);
    }
    return sel;
}

void LaneGrid::update(glm::vec2 mousePos, bool leftClicked) {
    m_hoveredLane = laneAt(mousePos);
    if (leftClicked && m_hoveredLane != -1) {
        toggleLane(m_hoveredLane); // Multiple lanes allowed (US-14)
    }
}

void LaneGrid::draw() {
    Renderer& ren = Renderer::instance();
    const float W = Constants::FIELD_X1 - Constants::FIELD_X0;

    for (int i = 0; i < Constants::NUM_LANES; ++i) {
        float y = Constants::laneY(i);

        if (m_selected[i]) {
            // Selected lane: highlighted with bright golden/cyan border & glow (US-14)
            ren.drawRect(Constants::FIELD_X0, y, W, Constants::LANE_HEIGHT, glm::vec4(0.20f, 0.70f, 1.0f, 0.18f));
            ren.drawRect(Constants::FIELD_X0, y, W, 3.0f, glm::vec4(0.30f, 0.85f, 1.0f, 0.95f));
            ren.drawRect(Constants::FIELD_X0, y + Constants::LANE_HEIGHT - 3.0f, W, 3.0f, glm::vec4(0.30f, 0.85f, 1.0f, 0.95f));
        } else if (i == m_hoveredLane) {
            // Hovered lane: subtle white translucent highlight (US-13)
            ren.drawRect(Constants::FIELD_X0, y, W, Constants::LANE_HEIGHT, glm::vec4(1.0f, 1.0f, 1.0f, 0.08f));
        }
    }
}
