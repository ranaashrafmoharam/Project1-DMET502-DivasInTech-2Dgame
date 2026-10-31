#include "PlayerCards.h"
// ====================================================================
// FILE:     src/ui/PlayerCards.cpp
// OWNER:    Member 4
// TASKS:    M4-06
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   The three player cards (Melee, Ranged, Defense).
//
// RESPONSIBILITIES:
//   - Locked = grayscale and no hover/click
//   - Unlocked = normal colours, hover and selectable
//   - Auto-unlock when coins reach the cost, auto re-lock when below
//   - Show which card is selected
//
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - update(coins, mouse, click)
//   - draw()
//   - getSelectedCard()
//
// INTERACTS WITH: Input, HUD, PlayerPlacement, Renderer
// ====================================================================
