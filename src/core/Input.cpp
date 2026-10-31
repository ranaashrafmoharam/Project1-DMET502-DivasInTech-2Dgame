#include "Input.h"
// ====================================================================
// FILE:     src/core/Input.cpp
// OWNER:    Member 1
// TASKS:    M1-01, M1-04
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   Wraps raw GLFW callbacks into a simple query API so game code
//   never touches GLFW directly.
//
// RESPONSIBILITIES:
//   - Track mouse position in world coordinates
//   - Track left-click events (pressed this frame)
//   - Track W / Up, S / Down and Esc presses
//
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - getMouseWorldPos()
//   - wasLeftClicked()
//   - jumpPressed()
//   - duckPressed()
//   - escPressed()
//
// INTERACTS WITH: LaneGrid, PlayerCards, Menus, Game
// ====================================================================
