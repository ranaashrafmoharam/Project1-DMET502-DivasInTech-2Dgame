// ====================================================================
// FILE:     src/world/PlayerPlacement.cpp
// OWNER:    Member 1
// TASKS:    M1-05
// STATUS:   IMPLEMENTED
// ====================================================================

#include "PlayerPlacement.h"
#include "../entities/Player.h"
#include <iostream>

int PlayerPlacement::getCardCost(CardType type) {
    switch (type) {
        case CardType::MELEE:   return Constants::COST_MELEE;
        case CardType::RANGED:  return Constants::COST_RANGED;
        case CardType::DEFENSE: return Constants::COST_DEFENSE;
        default:                return 0;
    }
}

Entity* PlayerPlacement::tryPlace(CardType cardType, int lane, int& currentCoins) {
    if (cardType == CardType::NONE) {
        return nullptr;
    }

    if (lane < 0 || lane >= Constants::LANE_COUNT) {
        return nullptr;
    }

    int cost = getCardCost(cardType);
    if (currentCoins < cost) {
        std::cout << "[PlayerPlacement] Not enough coins! Need " << cost << ", have " << currentCoins << std::endl;
        return nullptr;
    }

    // Deduct cost
    currentCoins -= cost;

    Player* newPlayer = new Player();
    switch (cardType) {
        case CardType::MELEE:
            newPlayer->init(UnitType::MeleePlayer, lane, Constants::PLAYER_SPAWN_X);
            break;
        case CardType::RANGED:
            newPlayer->init(UnitType::RangedPlayer, lane, Constants::PLAYER_SPAWN_X);
            break;
        case CardType::DEFENSE:
            newPlayer->init(UnitType::DefensePlayer, lane, Constants::PLAYER_SPAWN_X);
            break;
        default:
            delete newPlayer;
            break;
    }

    if (newPlayer) {
        std::cout << "[PlayerPlacement] Placed unit in lane " << lane << ". Coins remaining: " << currentCoins << std::endl;
    }

    return newPlayer;
}
