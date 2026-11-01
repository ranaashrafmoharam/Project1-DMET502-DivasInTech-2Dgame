#pragma once
// ====================================================================
// FILE:     src/ui/PlayerCards.h
// OWNER:    Member 4
// TASKS:    M4-06
// The three defender cards (Kai, Zane, Cole) with grayscale lock,
// auto unlock/relock, hover highlight, and selection state.
// ====================================================================

#include <glm/glm.hpp>
#include <string>

struct CardData {
    int id;               // 0: Melee, 1: Ranged, 2: Defense
    std::string name;     // "KAI (MELEE)", "ZANE (RANGED)", "COLE (DEFENSE)"
    std::string sprite;   // "kai", "zane", "cole"
    int cost;             // 10, 20, 30
    std::string desc;     // "5 HP, 1 Dmg", "5 HP, Shurikens", "7 HP, Blocker"

    float x = 0.0f;
    float y = 0.0f;
    float w = 150.0f;
    float h = 100.0f;

    bool isUnlocked = false;
    bool isHovered = false;
};

class PlayerCards {
public:
    static PlayerCards& instance();

    void init();
    void update(int coins, glm::vec2 mousePos, bool leftClicked);
    void draw();

    int getSelectedCard() const { return m_selectedIndex; }
    void clearSelection() { m_selectedIndex = -1; }

    int getCardCost(int cardIndex) const;

private:
    PlayerCards() = default;

    CardData m_cards[3];
    int m_selectedIndex = -1;
    float m_animTime = 0.0f;
};
