#pragma once
// ====================================================================
// FILE:     src/world/LaneGrid.h
// OWNER:    Member 1
// TASKS:    M1-04
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   The lane system: converts the mouse into a lane and tracks hover
//   and selection.
//
// RESPONSIBILITIES:
//   - Mouse position to lane index
//   - Track hovered lane
//   - Left click toggles lane selected; multiple lanes allowed
//   - Provide the selected-lane list for jump/duck
//
// PLANNED PUBLIC INTERFACE:
//   - update(mouse, click)
//   - isHovered(lane)
//   - isSelected(lane)
//   - getSelectedLanes()
//   - clear()
//
// INTERACTS WITH: Input, LaneHighlight, Game, Player
// ====================================================================

#include <vector>
#include <glm/glm.hpp>
#include "../core/Constants.h"

class LaneGrid {
public:
    LaneGrid();

    void update(const glm::vec2& mousePos, bool leftClicked);

    int getHoveredLane() const { return m_hoveredLane; }
    bool isHovered(int lane) const;
    bool isSelected(int lane) const;
    const std::vector<int>& getSelectedLanes() const { return m_selectedLanes; }

    void toggleLaneSelection(int lane);
    void selectLane(int lane);
    void deselectLane(int lane);
    void clear(); // Deselects all lanes

    // Helper: converts screen Y coordinate to lane index (0 .. LANE_COUNT-1), or -1 if outside
    static int getLaneFromY(float y);

private:
    int m_hoveredLane;
    bool m_laneSelected[Constants::LANE_COUNT];
    std::vector<int> m_selectedLanes;

    void updateSelectedList();
};
