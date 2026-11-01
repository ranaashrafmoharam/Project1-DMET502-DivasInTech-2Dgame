#include "PlayerPlacement.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include "../ui/PlayerCards.h"

PlayerPlacement& PlayerPlacement::instance() {
    static PlayerPlacement pp;
    return pp;
}

bool PlayerPlacement::tryPlacePlayer(int selectedCard, int targetLane, int& currentCoins,
                                     std::vector<std::shared_ptr<Player>>& players) {
    if (selectedCard < 0 || targetLane < 0 || targetLane >= Constants::NUM_LANES) {
        return false;
    }

    int cost = PlayerCards::instance().getCardCost(selectedCard);
    if (currentCoins < cost) {
        return false;
    }

    // Deduct cost
    currentCoins -= cost;

    // Determine type
    EntityType pType = EntityType::PLAYER_MELEE;
    if (selectedCard == 1) pType = EntityType::PLAYER_RANGED;
    else if (selectedCard == 2) pType = EntityType::PLAYER_DEFENSE;

    // Spawn at leftmost position of that lane (US-23)
    float spawnX = Constants::FIELD_X0 + 15.0f;
    auto newPlayer = std::make_shared<Player>(pType, targetLane, spawnX);
    players.push_back(newPlayer);

    AudioManager::instance().playSound(SoundID::PLAYER_PLACE);
    return true;
}
