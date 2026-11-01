#pragma once
// ====================================================================
// FILE:     src/ui/HUD.h
// OWNER:    Member 4
// TASKS:    M4-05
// In-game HUD: Golden Shard currency counter, wave info, controls hint.
// ====================================================================

#include <string>

class HUD {
public:
    static HUD& instance();

    void setCoins(int n) { m_coins = n; }
    int getCoins() const { return m_coins; }

    void update(float dt);
    void draw(int currentWave, int totalWaves, int enemiesRemaining, const std::string& waveName);

private:
    HUD() = default;

    int m_coins = 30;
    float m_animTime = 0.0f;
};
