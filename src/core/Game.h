#pragma once
// ====================================================================
// FILE:     src/core/Game.h
// OWNER:    Member 1
// TASKS:    M1-01, M1-03, M1-05, M1-11
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   The heart of the program. Owns the state machine, the main loop,
//   all entity lists and the reset logic. Every other system is
//   called from here.
// ====================================================================

#include <GLFW/glfw3.h>
#include <vector>
#include "GameState.h"
#include "Constants.h"
#include "../entities/Entity.h"
#include "../world/LaneGrid.h"
#include "../world/WaveManager.h"
#include "../world/CollisionSystem.h"
#include "../world/PlayerPlacement.h"

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void shutdown();

    void setState(GameState newState);
    GameState getState() const { return m_state; }

    void reset();
    void update(float dt);
    void render();

    // Entity and Economy accessors
    int getCoins() const { return m_coins; }
    void addCoins(int amount) { m_coins += amount; }
    bool spendCoins(int amount);

    CardType getSelectedCard() const { return m_selectedCard; }
    void setSelectedCard(CardType card) { m_selectedCard = card; }

    LaneGrid& getLaneGrid() { return m_laneGrid; }
    WaveManager& getWaveManager() { return m_waveManager; }
    CollisionSystem& getCollisionSystem() { return m_collisionSystem; }

private:
    GLFWwindow* m_window;
    GameState m_state;
    int m_coins;
    CardType m_selectedCard;

    LaneGrid m_laneGrid;
    WaveManager m_waveManager;
    CollisionSystem m_collisionSystem;

    std::vector<Entity*> m_players;
    std::vector<Entity*> m_enemies;
    std::vector<Entity*> m_projectiles;
    std::vector<Entity*> m_collectibles;

    void processInput();
    void cleanDeadEntities();
    void renderPlayfield();
    void renderEntities();
    void renderHUD();
    void renderOverlay();
};
