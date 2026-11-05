#pragma once
// ====================================================================

class LaneHighlight
{
public:
    void draw(int lane, bool hovered, bool selected);
};
// FILE:     src/scene/LaneHighlight.h
// OWNER:    Member 3
// TASKS:    M3-11
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Draws the lane hover and selected-lane highlights.
//
// RESPONSIBILITIES:
//   - Highlight the full lane under the mouse
//   - Different locked look for selected lanes
//   - Stay visible against the background
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - draw(laneGrid)
//
// INTERACTS WITH: LaneGrid, Game
// ====================================================================
