#pragma once
// ====================================================================
// FILE:     src/world/LaneGrid.h
// Converts mouse position to lane, tracks hovered lane, toggles
// selection (multiple lanes allowed) for jump/duck/placement.
// ====================================================================

#include <glm/glm.hpp>
#include <vector>
#include "../core/Constants.h"

class LaneGrid {
public:
    static LaneGrid& instance();

    void init();
    void reset();

    void update(glm::vec2 mousePos, bool leftClicked);
    void draw();

    int getHoveredLane() const { return m_hoveredLane; }
    bool isLaneSelected(int lane) const;
    void toggleLane(int lane);
    void selectSingleLane(int lane);
    void clearSelection();

    std::vector<int> getSelectedLanes() const;
    int laneAt(glm::vec2 worldPos) const;

private:
    LaneGrid() = default;

    int m_hoveredLane = -1;
    bool m_selected[Constants::NUM_LANES] = {false};
};
