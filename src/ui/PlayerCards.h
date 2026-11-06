#pragma once

class PlayerCards
{
public:
    void update(int coins, float mouseX, float mouseY, bool clicked);
    void draw() const;

    bool canSelect(int card, int coins) const;
    bool trySpendSelected(int& coins) const;
    bool trySpendCard(int card, int& coins) const;
    void selectCard(int card);
    int getSelectedCard() const { return selectedCard; }
    int getCost(int card) const;

private:
    int selectedCard = 0;
    int hoveredCard = -1;
    int availableCoins = 0;
};
