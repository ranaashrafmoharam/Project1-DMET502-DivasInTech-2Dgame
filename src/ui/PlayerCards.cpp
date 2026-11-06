#include "PlayerCards.h"
#include "scene/Scene.h"
#include <cstdio>

namespace
{
const float startX = 335.0f;
const float cardY = 20.0f;
const float cardWidth = 140.0f;
const float cardHeight = 78.0f;
const float cardSpacing = 168.0f;
const char* cardNames[] = {"KAI", "ZANE", "COLE"};
const char* cardTypes[] = {"MELEE", "RANGED", "DEFENSE"};
const char* cardImages[] = {
    "assets/textures/kai_card_new.bmp",
    "assets/textures/zane_card_new.bmp",
    "assets/textures/cole_card_new.bmp"
};
}

int PlayerCards::getCost(int card) const
{
    return card == 0 ? 10 : card == 1 ? 20 : 30;
}

bool PlayerCards::canSelect(int card, int coins) const
{
    return card >= 0 && card < 3 && coins >= getCost(card);
}

bool PlayerCards::trySpendSelected(int& coins) const
{
    const int cost = getCost(selectedCard);
    if (!canSelect(selectedCard, coins))
        return false;
    coins -= cost;
    return true;
}

bool PlayerCards::trySpendCard(int card, int& coins) const
{
    if (!canSelect(card, coins))
        return false;
    coins -= getCost(card);
    return true;
}

void PlayerCards::selectCard(int card)
{
    if (canSelect(card, availableCoins))
        selectedCard = card;
}

void PlayerCards::update(int coins, float mouseX, float mouseY, bool clicked)
{
    availableCoins = coins;
    hoveredCard = -1;
    for (int card = 0; card < 3; ++card)
    {
        const float cardX = startX + card * cardSpacing;
        if (canSelect(card, coins) &&
            mouseX >= cardX && mouseX <= cardX + cardWidth &&
            mouseY >= cardY && mouseY <= cardY + cardHeight)
        {
            hoveredCard = card;
            if (clicked)
                selectedCard = card;
        }
    }

    if (!canSelect(selectedCard, coins))
        selectedCard = 0;
}

void PlayerCards::draw() const
{
    M3Draw::quad(260.0f, 8.0f, 650.0f, 110.0f, 0.08f, 0.07f, 0.14f, 0.95f);
    M3Draw::quad(262.0f, 10.0f, 646.0f, 106.0f, 0.78f, 0.65f, 0.28f, 0.95f);
    M3Draw::quad(264.0f, 12.0f, 642.0f, 102.0f, 0.12f, 0.11f, 0.18f, 0.95f);

    for (int card = 0; card < 3; ++card)
    {
        const float cardX = startX + card * cardSpacing;
        const bool selected = selectedCard == card;
        const bool unlocked = canSelect(card, availableCoins);

        if (selected || hoveredCard == card)
            M3Draw::quad(cardX - 3.0f, cardY - 3.0f, cardWidth + 6.0f, cardHeight + 6.0f,
                         selected ? 1.0f : 0.55f, selected ? 0.82f : 0.85f,
                         selected ? 0.22f : 0.35f, 1.0f);
        M3Draw::image(cardImages[card], cardX, cardY, cardWidth, cardHeight, unlocked ? 1.0f : 0.35f);
        M3Draw::text(cardX + 6.0f, cardY + 82.0f, cardNames[card], 1.15f,
                     unlocked ? 1.0f : 0.45f, unlocked ? 0.95f : 0.45f, unlocked ? 0.65f : 0.45f);
        M3Draw::text(cardX + 50.0f, cardY + 82.0f, cardTypes[card], 0.95f,
                     unlocked ? 0.75f : 0.45f, unlocked ? 0.90f : 0.45f, 1.0f,
                     unlocked ? 1.0f : 0.7f);
        char costText[32];
        std::snprintf(costText, sizeof(costText), "%d SHARDS", getCost(card));
        M3Draw::text(cardX + 83.0f, cardY + 68.0f, unlocked ? costText : "LOCKED",
                     0.95f, 1.0f, 0.82f, 0.25f);
    }
}
