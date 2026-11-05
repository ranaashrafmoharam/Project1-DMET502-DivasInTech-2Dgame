// ====================================================================
// FILE:     src/world/PlayerPlacement.cpp
// OWNER:    Member 1
// TASKS:    M1-05
// STATUS:   IMPLEMENTED
// ====================================================================

#include "PlayerPlacement.h"
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

    float spawnX = Constants::PLAYER_SPAWN_X;
    float spawnY = Constants::getLaneCenterY(lane);

    Entity* newPlayer = nullptr;

    switch (cardType) {
        case CardType::MELEE: {
            newPlayer = new Entity(
                EntityType::PLAYER_MELEE,
                lane,
                spawnX,
                spawnY,
                Constants::HP_PLAYER_MELEE,
                Constants::SPEED_PLAYER_MELEE
            );
            newPlayer->attackDamage = Constants::DMG_PLAYER_MELEE;
            newPlayer->attackCooldown = Constants::ATTACK_COOLDOWN_MELEE;
            break;
        }
        case CardType::RANGED: {
            newPlayer = new Entity(
                EntityType::PLAYER_RANGED,
                lane,
                spawnX,
                spawnY,
                Constants::HP_PLAYER_RANGED,
                Constants::SPEED_PLAYER_RANGED
            );
            newPlayer->attackDamage = Constants::DMG_PLAYER_RANGED;
            newPlayer->attackCooldown = Constants::ATTACK_COOLDOWN_RANGED;
            break;
        }
        case CardType::DEFENSE: {
            newPlayer = new Entity(
                EntityType::PLAYER_DEFENSE,
                lane,
                spawnX,
                spawnY,
                Constants::HP_PLAYER_DEFENSE,
                Constants::SPEED_PLAYER_DEFENSE // 0.0f
            );
            newPlayer->attackDamage = Constants::DMG_PLAYER_DEFENSE; // 0
            break;
        }
        default:
            break;
    }

    if (newPlayer) {
        std::cout << "[PlayerPlacement] Placed unit in lane " << lane << ". Coins remaining: " << currentCoins << std::endl;
    }

    return newPlayer;
}
