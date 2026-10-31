#include "LaneGrid.h"
// ====================================================================
// FILE:     src/world/LaneGrid.cpp
// OWNER:    Member 1
// TASKS:    M1-04
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
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
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - update(mouse, click)
//   - isHovered(lane)
//   - isSelected(lane)
//   - getSelectedLanes()
//   - clear()
//
// INTERACTS WITH: Input, LaneHighlight, Game, Player
// ====================================================================
