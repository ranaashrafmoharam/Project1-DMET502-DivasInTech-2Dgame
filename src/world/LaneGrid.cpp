// ====================================================================
// FILE:     src/world/LaneGrid.cpp
// OWNER:    Member 1
// TASKS:    M1-04
// STATUS:   IMPLEMENTED
// ====================================================================

#include "LaneGrid.h"
#include <algorithm>

LaneGrid::LaneGrid()
    : m_hoveredLane(-1)
{
    for (int i = 0; i < Constants::LANE_COUNT; ++i) {
        m_laneSelected[i] = false;
    }
}

int LaneGrid::getLaneFromY(float y) {
    if (y < Constants::TOP_BAR_HEIGHT || y >= Constants::SCREEN_HEIGHT) {
        return -1;
    }
    int lane = static_cast<int>((y - Constants::TOP_BAR_HEIGHT) / Constants::LANE_HEIGHT);
    if (lane >= 0 && lane < Constants::LANE_COUNT) {
        return lane;
    }
    return -1;
}

void LaneGrid::update(const glm::vec2& mousePos, bool leftClicked) {
    // Only detect hover inside the playable area (x >= HOME_BASE_X)
    if (mousePos.x >= Constants::HOME_BASE_X && mousePos.x <= Constants::RIGHT_EDGE_X) {
        m_hoveredLane = getLaneFromY(mousePos.y);
    } else {
        m_hoveredLane = -1;
    }

    // Left click toggles selection on the hovered lane
    if (leftClicked && m_hoveredLane != -1) {
        toggleLaneSelection(m_hoveredLane);
    }
}

bool LaneGrid::isHovered(int lane) const {
    return (lane >= 0 && lane < Constants::LANE_COUNT && m_hoveredLane == lane);
}

bool LaneGrid::isSelected(int lane) const {
    if (lane >= 0 && lane < Constants::LANE_COUNT) {
        return m_laneSelected[lane];
    }
    return false;
}

void LaneGrid::toggleLaneSelection(int lane) {
    if (lane >= 0 && lane < Constants::LANE_COUNT) {
        m_laneSelected[lane] = !m_laneSelected[lane];
        updateSelectedList();
    }
}

void LaneGrid::selectLane(int lane) {
    if (lane >= 0 && lane < Constants::LANE_COUNT) {
        m_laneSelected[lane] = true;
        updateSelectedList();
    }
}

void LaneGrid::deselectLane(int lane) {
    if (lane >= 0 && lane < Constants::LANE_COUNT) {
        m_laneSelected[lane] = false;
        updateSelectedList();
    }
}

void LaneGrid::clear() {
    for (int i = 0; i < Constants::LANE_COUNT; ++i) {
        m_laneSelected[i] = false;
    }
    m_selectedLanes.clear();
}

void LaneGrid::updateSelectedList() {
    m_selectedLanes.clear();
    for (int i = 0; i < Constants::LANE_COUNT; ++i) {
        if (m_laneSelected[i]) {
            m_selectedLanes.push_back(i);
        }
    }
}
