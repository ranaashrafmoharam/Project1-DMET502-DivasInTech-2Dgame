#pragma once
// ====================================================================
// FILE:     src/world/PlayerPlacement.h
// OWNER:    Member 1
// TASKS:    M1-05
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Places a purchased player on the grid.
//
// RESPONSIBILITIES:
//   - Require a selected card and a selected lane
//   - Check and deduct coins
//   - Spawn the player at the leftmost x of the lane
//   - Lock the player to that lane
//
// PLANNED PUBLIC INTERFACE:
//   - tryPlace(cardType, lane, coins)
//
// INTERACTS WITH: PlayerCards, LaneGrid, Player, HUD
// ====================================================================

#include "../entities/Entity.h"

enum class CardType {
    NONE = -1,
    MELEE = 0,
    RANGED = 1,
    DEFENSE = 2
};

class PlayerPlacement {
public:
    static int getCardCost(CardType type);

    // Attempts to place a unit. If successful, deducts coins and returns new Entity*.
    // If invalid lane, card, or insufficient coins, returns nullptr without deducting coins.
    static Entity* tryPlace(CardType cardType, int lane, int& currentCoins);
};
