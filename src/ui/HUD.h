#pragma once

#include <string>

struct GameplayHudState
{
    int coins = 30;
    int currentWave = 1;
    int totalWaves = 3;
    int enemiesRemaining = 0;
    int gateMaxHealth = 100;
    int gateHealth = 100;
    std::string notification;
    float notificationAlpha = 0.0f;
};

class HUD
{
public:
    void draw(const GameplayHudState& state) const;
};
