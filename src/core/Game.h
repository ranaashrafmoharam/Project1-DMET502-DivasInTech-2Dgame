#pragma once
// ====================================================================
// FILE:     src/core/Game.h
// The heart of the program: state machine, main loop, entity lists,
// win/lose detection, reset, update, and render.
// ====================================================================

#include <vector>
#include <memory>
#include "GameState.h"
#include "../entities/Player.h"
#include "../entities/Enemy.h"
#include "../entities/Projectile.h"
#include "../entities/Collectible.h"

struct GLFWwindow;

class Game {
public:
    Game() = default;
    ~Game() = default;

    bool init(GLFWwindow* window);
    void run();

    void setState(GameState s);
    GameState getState() const { return m_state; }

    void startCutscene();
    void reset();
    void quit();

    void update(float dt);
    void render();

    int getCoins() const { return m_coins; }
    void addCoins(int amount) { m_coins += amount; }

    void enableAutoTest(bool enable) { m_autoTest = enable; }
    static bool saveScreenshotBMP(const std::string& path, int width, int height);

private:
    GLFWwindow* m_window = nullptr;
    GameState m_state = GameState::MAIN_MENU;
    bool m_autoTest = false;
    int m_autoTestFrame = 0;

    int m_coins = 30; // Starting coins (US-22: enough for 3 Melee players)

    std::vector<std::shared_ptr<Player>> m_players;
    std::vector<std::shared_ptr<Enemy>> m_enemies;
    std::vector<std::shared_ptr<Projectile>> m_projectiles;
    std::vector<std::shared_ptr<Collectible>> m_collectibles;
};
