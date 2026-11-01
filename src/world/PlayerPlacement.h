#pragma once
// ====================================================================
// FILE:     src/world/PlayerPlacement.h
// Handles checking affordability, deducting coins, and spawning
// defenders at the leftmost position of a selected lane.
// ====================================================================

#include <vector>
#include <memory>
#include "../entities/Player.h"

class PlayerPlacement {
public:
    static PlayerPlacement& instance();

    bool tryPlacePlayer(int selectedCard, int targetLane, int& currentCoins,
                        std::vector<std::shared_ptr<Player>>& players);

private:
    PlayerPlacement() = default;
};
