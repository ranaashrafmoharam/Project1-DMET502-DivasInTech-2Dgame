// ====================================================================
// FILE:     src/core/Game.cpp
// OWNER:    Member 1
// TASKS:    M1-01, M1-03, M1-05, M1-11
// STATUS:   IMPLEMENTED
// ====================================================================

#if __has_include(<glad/glad.h>)
#include <glad/glad.h>
#elif __has_include(<glad/gl.h>)
#include <glad/gl.h>
#endif

#include "Game.h"
#include "Input.h"
#include "Renderer.h"
#include "../entities/Player.h"
#include "../entities/Enemy.h"
#include "../entities/Projectile.h"
#include <iostream>
#include <string>

struct SimpleButton {
    float x, y, width, height;
    std::string text;
    glm::vec4 color;
    glm::vec4 hoverColor;

    bool isHovered(const glm::vec2& mouse) const {
        return (mouse.x >= x && mouse.x <= x + width &&
                mouse.y >= y && mouse.y <= y + height);
    }

    void draw(Renderer& renderer, const glm::vec2& mouse) const {
        bool hovered = isHovered(mouse);
        renderer.drawQuad(x, y, width, height, hovered ? hoverColor : color);
        renderer.drawBorder(x, y, width, height, 2.0f, hovered ? glm::vec4(1.0f, 1.0f, 0.3f, 1.0f) : glm::vec4(0.5f, 0.5f, 0.6f, 0.8f));
        renderer.drawTextCentered(text, x + width * 0.5f, y + height * 0.5f, 2.0f, glm::vec4(1.0f));
    }
};

Game::Game()
    : m_window(nullptr)
    , m_state(GameState::MAIN_MENU) // Start on Main Menu as required
    , m_coins(Constants::STARTING_COINS)
    , m_selectedCard(CardType::MELEE)
{
}

Game::~Game() {
    shutdown();
}

bool Game::init() {
    if (!glfwInit()) {
        std::cerr << "[Game] Failed to initialize GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(
        Constants::SCREEN_WIDTH,
        Constants::SCREEN_HEIGHT,
        Constants::WINDOW_TITLE.c_str(),
        nullptr,
        nullptr
    );

    if (!m_window) {
        std::cerr << "[Game] Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable VSync

    // Initialize GLAD
#if defined(GLAD_GL_H_) || defined(GLAD_OPTION_GLX)
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "[Game] Failed to initialize GLAD!" << std::endl;
        return false;
    }
#else
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[Game] Failed to initialize GLAD!" << std::endl;
        return false;
    }
#endif

    glViewport(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);

    // Setup engine modules
    Input::init(m_window);

    if (!Renderer::getInstance().init()) {
        std::cerr << "[Game] Failed to initialize Renderer!" << std::endl;
        return false;
    }

    reset();
    m_state = GameState::MAIN_MENU;

    std::cout << "[Game] Initialization successful. OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    return true;
}

void Game::shutdown() {
    reset();
    Renderer::getInstance().shutdown();

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Game::setState(GameState newState) {
    m_state = newState;
    std::cout << "[Game] State changed to: " << (int)m_state << std::endl;
}

void Game::reset() {
    for (auto* p : m_players) delete p;
    m_players.clear();

    for (auto* e : m_enemies) delete e;
    m_enemies.clear();

    for (auto* proj : m_projectiles) delete proj;
    m_projectiles.clear();

    for (auto* c : m_collectibles) delete c;
    m_collectibles.clear();

    m_coins = Constants::STARTING_COINS;
    m_selectedCard = CardType::MELEE;

    m_laneGrid.clear();
    m_waveManager.reset();
    m_collisionSystem.reset();
}

bool Game::spendCoins(int amount) {
    if (m_coins >= amount) {
        m_coins -= amount;
        return true;
    }
    return false;
}

void Game::processInput() {
    glm::vec2 mousePos = Input::getMouseWorldPos();
    bool leftClicked = Input::wasLeftClicked();

    // Key 'P' Demonstration Cheat: Add 50 coins whenever pressed!
    if (Input::pPressed()) {
        m_coins += 50;
        std::cout << "[Demo] Key 'P' pressed! Added 50 coins. Total: " << m_coins << std::endl;
    }

    // Escape toggles pause while playing, or unpauses
    if (Input::escPressed()) {
        if (m_state == GameState::PLAYING) {
            setState(GameState::PAUSED);
        } else if (m_state == GameState::PAUSED) {
            setState(GameState::PLAYING);
        }
    }

    // Restart key (R) from Won / Lost / Paused
    if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) {
        if (m_state == GameState::WON || m_state == GameState::LOST || m_state == GameState::PAUSED) {
            reset();
            setState(GameState::PLAYING);
            return;
        }
    }

    // ================================================================
    // Menu Button Interactions (MAIN_MENU, PAUSED, WON, LOST)
    // ================================================================
    if (m_state == GameState::MAIN_MENU) {
        SimpleButton btnStart{ 490.0f, 260.0f, 300.0f, 50.0f, "START GAME", glm::vec4(0.15f, 0.55f, 0.25f, 1.0f), glm::vec4(0.2f, 0.75f, 0.35f, 1.0f) };
        SimpleButton btnExit { 490.0f, 330.0f, 300.0f, 50.0f, "EXIT",       glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        if (leftClicked) {
            if (btnStart.isHovered(mousePos)) {
                reset();
                setState(GameState::PLAYING);
                return;
            }
            if (btnExit.isHovered(mousePos)) {
                glfwSetWindowShouldClose(m_window, true);
                return;
            }
        }
        return;
    }

    if (m_state == GameState::PAUSED) {
        SimpleButton btnResume { 490.0f, 250.0f, 300.0f, 45.0f, "RESUME",    glm::vec4(0.2f, 0.4f, 0.7f, 1.0f), glm::vec4(0.3f, 0.55f, 0.9f, 1.0f) };
        SimpleButton btnRestart{ 490.0f, 310.0f, 300.0f, 45.0f, "RESTART",   glm::vec4(0.7f, 0.45f, 0.15f, 1.0f), glm::vec4(0.9f, 0.6f, 0.2f, 1.0f) };
        SimpleButton btnMain   { 490.0f, 370.0f, 300.0f, 45.0f, "MAIN MENU", glm::vec4(0.3f, 0.35f, 0.4f, 1.0f), glm::vec4(0.4f, 0.45f, 0.55f, 1.0f) };
        SimpleButton btnExit   { 490.0f, 430.0f, 300.0f, 45.0f, "EXIT",      glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        if (leftClicked) {
            if (btnResume.isHovered(mousePos))  { setState(GameState::PLAYING); return; }
            if (btnRestart.isHovered(mousePos)) { reset(); setState(GameState::PLAYING); return; }
            if (btnMain.isHovered(mousePos))    { reset(); setState(GameState::MAIN_MENU); return; }
            if (btnExit.isHovered(mousePos))    { glfwSetWindowShouldClose(m_window, true); return; }
        }
        return;
    }

    if (m_state == GameState::WON || m_state == GameState::LOST) {
        SimpleButton btnRestart{ 490.0f, 320.0f, 300.0f, 50.0f, "RESTART",   glm::vec4(0.2f, 0.55f, 0.25f, 1.0f), glm::vec4(0.3f, 0.75f, 0.35f, 1.0f) };
        SimpleButton btnMain   { 490.0f, 390.0f, 300.0f, 50.0f, "MAIN MENU", glm::vec4(0.3f, 0.35f, 0.4f, 1.0f), glm::vec4(0.4f, 0.45f, 0.55f, 1.0f) };
        SimpleButton btnExit   { 490.0f, 460.0f, 300.0f, 50.0f, "EXIT",      glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        if (leftClicked) {
            if (btnRestart.isHovered(mousePos)) { reset(); setState(GameState::PLAYING); return; }
            if (btnMain.isHovered(mousePos))    { reset(); setState(GameState::MAIN_MENU); return; }
            if (btnExit.isHovered(mousePos))    { glfwSetWindowShouldClose(m_window, true); return; }
        }
        return;
    }

    if (m_state != GameState::PLAYING) {
        return;
    }

    // ================================================================
    // Gameplay Interactions (PLAYING)
    // ================================================================

    // Coin Pickup by Mouse (Hover OR Click collects coin)
    for (auto* c : m_collectibles) {
        if (c && c->alive) {
            Bounds b = c->getBounds();
            // Generous pickup radius around the coin
            bool mouseOver = (mousePos.x >= b.x - 20.0f && mousePos.x <= b.x + b.width + 20.0f &&
                              mousePos.y >= b.y - 20.0f && mousePos.y <= b.y + b.height + 20.0f);
            if (mouseOver && (leftClicked || true)) { // Click or hover collects it
                c->alive = false;
                m_coins += Constants::COIN_DROP_VALUE;
                std::cout << "[Game] Coin collected by mouse! Total coins: " << m_coins << std::endl;
            }
        }
    }

    // Keyboard shortcuts for Card Selection (Keys 1, 2, 3)
    if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS) m_selectedCard = CardType::MELEE;
    if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS) m_selectedCard = CardType::RANGED;
    if (glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS) m_selectedCard = CardType::DEFENSE;

    // 1. Lane picking & selection
    m_laneGrid.update(mousePos, leftClicked);

    // 2. Jump & Duck toggle commands applied to all selected lanes
    if (Input::jumpPressed()) {
        for (int lane : m_laneGrid.getSelectedLanes()) {
            for (auto* p : m_players) {
                if (p && p->alive && p->laneIndex == lane) {
                    p->jump();
                }
            }
        }
    }

    if (Input::duckPressed()) {
        for (int lane : m_laneGrid.getSelectedLanes()) {
            for (auto* p : m_players) {
                if (p && p->alive && p->laneIndex == lane) {
                    p->duck();
                }
            }
        }
    }

    // 3. Card click selection (Top Bar Cards)
    if (leftClicked && mousePos.y < Constants::TOP_BAR_HEIGHT) {
        if (mousePos.x >= 250.0f && mousePos.x <= 360.0f) m_selectedCard = CardType::MELEE;
        if (mousePos.x >= 380.0f && mousePos.x <= 490.0f) m_selectedCard = CardType::RANGED;
        if (mousePos.x >= 510.0f && mousePos.x <= 620.0f) m_selectedCard = CardType::DEFENSE;
    }

    // 4. Player placement on selected lane
    if (leftClicked && mousePos.y >= Constants::TOP_BAR_HEIGHT && m_selectedCard != CardType::NONE) {
        int targetLane = m_laneGrid.getHoveredLane();
        if (targetLane != -1 && m_laneGrid.isSelected(targetLane)) {
            Entity* newPlayer = PlayerPlacement::tryPlace(m_selectedCard, targetLane, m_coins);
            if (newPlayer) {
                m_players.push_back(newPlayer);
            }
        }
    }
}

void Game::update(float dt) {
    if (m_state != GameState::PLAYING) {
        return;
    }

    // Count alive enemies
    int aliveEnemiesCount = 0;
    for (const auto* e : m_enemies) {
        if (e && e->alive) aliveEnemiesCount++;
    }

    // Update Wave Spawner
    Entity* newEnemy = m_waveManager.update(dt, aliveEnemiesCount);
    if (newEnemy) {
        m_enemies.push_back(newEnemy);
    }

    // Update Players
    for (auto* p : m_players) {
        if (p && p->alive) {
            p->update(dt);
        }
    }

    // Update Enemies
    for (auto* e : m_enemies) {
        if (e && e->alive) {
            e->update(dt);
        }
    }

    // Update Projectiles
    for (auto* proj : m_projectiles) {
        if (proj && proj->alive) {
            proj->update(dt);
        }
    }

    // Update Collectibles
    for (auto* c : m_collectibles) {
        if (c && c->alive) {
            c->update(dt);
        }
    }

    // Resolve Collisions & Combat
    m_collisionSystem.update(m_players, m_enemies, m_projectiles, m_collectibles, m_coins, dt);

    // Defeated enemies spawn a collectible coin before removal
    for (auto* e : m_enemies) {
        if (e && !e->alive && e->health <= 0) {
            Entity* coin = new Entity(
                EntityType::COLLECTIBLE,
                e->laneIndex,
                e->position.x,
                e->position.y,
                1,
                0.0f
            );
            coin->size = glm::vec2(Constants::COLLECTIBLE_SIZE, Constants::COLLECTIBLE_SIZE);
            m_collectibles.push_back(coin);
            e->health = -999; // Marked so it doesn't drop duplicate coin
        }
    }

    // Check Win Condition
    if (m_waveManager.allWavesFinished() && aliveEnemiesCount == 0) {
        setState(GameState::WON);
        return;
    }

    // Check Lose Condition
    if (m_collisionSystem.hasEnemyReachedBase()) {
        setState(GameState::LOST);
        return;
    }

    cleanDeadEntities();
}

void Game::cleanDeadEntities() {
    auto removeDead = [](std::vector<Entity*>& vec) {
        for (auto it = vec.begin(); it != vec.end(); ) {
            if (*it && !(*it)->alive) {
                delete *it;
                it = vec.erase(it);
            } else {
                ++it;
            }
        }
    };

    removeDead(m_players);
    removeDead(m_enemies);
    removeDead(m_projectiles);
    removeDead(m_collectibles);
}

void Game::render() {
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_state == GameState::MAIN_MENU) {
        // Render Main Menu Screen
        Renderer& renderer = Renderer::getInstance();
        glm::vec2 mousePos = Input::getMouseWorldPos();

        // Decorative background banner
        renderer.drawQuad(0.0f, 0.0f, (float)Constants::SCREEN_WIDTH, (float)Constants::SCREEN_HEIGHT, glm::vec4(0.08f, 0.10f, 0.14f, 1.0f));
        renderer.drawQuad(0.0f, 120.0f, (float)Constants::SCREEN_WIDTH, 6.0f, glm::vec4(0.9f, 0.7f, 0.1f, 0.9f));

        // Title and Subtitles
        renderer.drawTextCentered("NINJAGO: 2D TOWER DEFENSE", Constants::SCREEN_WIDTH * 0.5f, 60.0f, 4.0f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));
        renderer.drawTextCentered("DMET 502 COMPUTER GRAPHICS PROJECT", Constants::SCREEN_WIDTH * 0.5f, 100.0f, 2.0f, glm::vec4(0.7f, 0.8f, 0.9f, 1.0f));

        // Buttons
        SimpleButton btnStart{ 490.0f, 240.0f, 300.0f, 55.0f, "START GAME", glm::vec4(0.15f, 0.55f, 0.25f, 1.0f), glm::vec4(0.2f, 0.75f, 0.35f, 1.0f) };
        SimpleButton btnExit { 490.0f, 320.0f, 300.0f, 55.0f, "EXIT",       glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        btnStart.draw(renderer, mousePos);
        btnExit.draw(renderer, mousePos);

        // Instructions Panel
        float panelY = 440.0f;
        renderer.drawQuad(240.0f, panelY, 800.0f, 230.0f, glm::vec4(0.14f, 0.16f, 0.22f, 0.95f));
        renderer.drawBorder(240.0f, panelY, 800.0f, 230.0f, 2.0f, glm::vec4(0.4f, 0.5f, 0.7f, 0.8f));

        renderer.drawTextCentered("=== CONTROLS & HOW TO PLAY ===", Constants::SCREEN_WIDTH * 0.5f, panelY + 25.0f, 2.0f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));
        renderer.drawText("1. CLICK LANES TO SELECT / TOGGLE THEM (GOLD BORDER).", 260.0f, panelY + 60.0f, 2.0f, glm::vec4(0.85f, 0.9f, 0.95f, 1.0f));
        renderer.drawText("2. CLICK CARDS OR PRESS 1, 2, 3 TO PLACE NINJAS ON SELECTED LANE.", 260.0f, panelY + 90.0f, 2.0f, glm::vec4(0.85f, 0.9f, 0.95f, 1.0f));
        renderer.drawText("3. PRESS W / UP TO JUMP (DODGE LOW SHOTS) - STAYS UNTIL TOGGLED.", 260.0f, panelY + 120.0f, 2.0f, glm::vec4(0.85f, 0.9f, 0.95f, 1.0f));
        renderer.drawText("4. PRESS S / DOWN TO DUCK (DODGE HIGH SHOTS) - STAYS UNTIL TOGGLED.", 260.0f, panelY + 150.0f, 2.0f, glm::vec4(0.85f, 0.9f, 0.95f, 1.0f));
        renderer.drawText("5. CLICK OR HOVER OVER DROPPED COINS TO PICK THEM UP.", 260.0f, panelY + 180.0f, 2.0f, glm::vec4(1.0f, 0.9f, 0.3f, 1.0f));
        renderer.drawText("6. DEMO CHEAT: PRESS P TO GET +50 FREE COINS AT ANY TIME!", 260.0f, panelY + 205.0f, 2.0f, glm::vec4(0.4f, 1.0f, 0.5f, 1.0f));

        return;
    }

    renderPlayfield();
    renderEntities();
    renderHUD();
    renderOverlay();
}

void Game::renderPlayfield() {
    Renderer& renderer = Renderer::getInstance();

    // Home Base (Leftmost safe area)
    renderer.drawQuad(
        0.0f,
        Constants::TOP_BAR_HEIGHT,
        Constants::HOME_BASE_WIDTH,
        Constants::PLAYFIELD_HEIGHT,
        glm::vec4(0.20f, 0.24f, 0.30f, 1.0f)
    );
    // Home Base label
    renderer.drawTextCentered("HOME BASE", Constants::HOME_BASE_WIDTH * 0.5f, Constants::TOP_BAR_HEIGHT + 20.0f, 2.0f, glm::vec4(0.6f, 0.75f, 0.9f, 0.9f));
    renderer.drawTextCentered("DEFEND!", Constants::HOME_BASE_WIDTH * 0.5f, Constants::TOP_BAR_HEIGHT + 45.0f, 2.0f, glm::vec4(0.9f, 0.3f, 0.3f, 0.9f));

    // Home Base boundary line (Red threshold)
    renderer.drawQuad(
        Constants::HOME_BASE_X - 4.0f,
        Constants::TOP_BAR_HEIGHT,
        4.0f,
        Constants::PLAYFIELD_HEIGHT,
        glm::vec4(0.95f, 0.2f, 0.2f, 0.95f)
    );

    // Lanes background & dividers
    for (int i = 0; i < Constants::LANE_COUNT; ++i) {
        float y = Constants::getLaneTopY(i);

        // Lane background
        glm::vec4 laneBgColor = (i % 2 == 0)
            ? glm::vec4(0.16f, 0.20f, 0.23f, 1.0f)
            : glm::vec4(0.13f, 0.17f, 0.20f, 1.0f);

        // Lane Hover Highlight
        if (m_laneGrid.isHovered(i)) {
            laneBgColor += glm::vec4(0.08f, 0.08f, 0.04f, 0.0f);
        }
        // Lane Selected Highlight
        if (m_laneGrid.isSelected(i)) {
            laneBgColor += glm::vec4(0.12f, 0.14f, 0.04f, 0.0f);
        }

        renderer.drawQuad(Constants::HOME_BASE_X, y, Constants::SCREEN_WIDTH - Constants::HOME_BASE_X, Constants::LANE_HEIGHT, laneBgColor);

        // Lane border highlight when selected
        if (m_laneGrid.isSelected(i)) {
            renderer.drawBorder(Constants::HOME_BASE_X, y, Constants::SCREEN_WIDTH - Constants::HOME_BASE_X, Constants::LANE_HEIGHT, 3.0f, glm::vec4(0.95f, 0.85f, 0.2f, 0.85f));
            renderer.drawText("SELECTED", Constants::HOME_BASE_X + 15.0f, y + 10.0f, 1.5f, glm::vec4(0.95f, 0.85f, 0.2f, 0.9f));
        }

        // Horizontal lane divider
        renderer.drawQuad(Constants::HOME_BASE_X, y + Constants::LANE_HEIGHT - 2.0f, Constants::SCREEN_WIDTH - Constants::HOME_BASE_X, 2.0f, glm::vec4(0.28f, 0.32f, 0.38f, 0.6f));
    }
}

void Game::renderEntities() {
    Renderer& renderer = Renderer::getInstance();

    // 1. Collectibles (Golden Coins)
    for (const auto* c : m_collectibles) {
        if (!c || !c->alive) continue;
        renderer.drawQuadCentered(c->position.x, c->position.y, c->size.x, c->size.y, glm::vec4(1.0f, 0.84f, 0.0f, 1.0f));
        renderer.drawBorder(c->position.x - c->size.x * 0.5f, c->position.y - c->size.y * 0.5f, c->size.x, c->size.y, 2.0f, glm::vec4(1.0f, 1.0f, 0.6f, 1.0f));
        renderer.drawTextCentered("COIN", c->position.x, c->position.y, 1.2f, glm::vec4(0.2f, 0.2f, 0.0f, 1.0f));
    }

    // 2. Players
    for (auto* p : m_players) {
        if (!p || !p->alive) continue;

        Bounds b = p->getBounds();
        if (Player* character = dynamic_cast<Player*>(p)) {
            if (p->shielded) {
                renderer.drawBorder(b.x - 4.0f, b.y - 4.0f, b.width + 8.0f, b.height + 8.0f,
                                    3.0f, glm::vec4(0.0f, 1.0f, 1.0f, 0.9f));
            }
            character->draw();
            const float hpPercent = character->healthFraction();
            renderer.drawQuad(b.x, b.y - 12.0f, b.width, 8.0f, glm::vec4(0.3f, 0.0f, 0.0f, 0.8f));
            renderer.drawQuad(b.x, b.y - 12.0f, b.width * hpPercent, 8.0f, glm::vec4(0.1f, 0.9f, 0.1f, 0.9f));
            renderer.drawBorder(b.x, b.y - 12.0f, b.width, 8.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
            continue;
        }

        glm::vec4 color(0.2f, 0.5f, 0.9f, 1.0f); // Melee (blue)
        std::string unitName = "MELEE";
        if (p->type == EntityType::PLAYER_RANGED) {
            color = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f); // Ranged (green)
            unitName = "RANGED";
        }
        if (p->type == EntityType::PLAYER_DEFENSE) {
            color = glm::vec4(0.5f, 0.6f, 0.7f, 1.0f); // Defense (shield)
            unitName = "SHIELD";
        }

        if (p->shielded) {
            // Cyan aura around shielded players
            renderer.drawBorder(b.x - 4.0f, b.y - 4.0f, b.width + 8.0f, b.height + 8.0f, 3.0f, glm::vec4(0.0f, 1.0f, 1.0f, 0.9f));
        }

        // Damage tint
        float hpPercent = (float)p->health / (float)p->maxHealth;
        glm::vec4 tint(1.0f, hpPercent, hpPercent, 1.0f);
        renderer.setTint(tint);
        renderer.drawQuad(b.x, b.y, b.width, b.height, color);
        renderer.resetShaderState();

        // Label unit name inside body
        renderer.drawTextCentered(unitName, b.x + b.width * 0.5f, b.y + b.height * 0.5f, 1.5f, glm::vec4(1.0f));

        // State indicator: JUMPING or DUCKING badge
        if (p->isJumping) {
            renderer.drawQuad(b.x, b.y - 28.0f, b.width, 12.0f, glm::vec4(0.2f, 0.7f, 1.0f, 0.9f));
            renderer.drawTextCentered("JUMPING", b.x + b.width * 0.5f, b.y - 22.0f, 1.2f, glm::vec4(1.0f));
        } else if (p->isDucking) {
            renderer.drawQuad(b.x, b.y - 28.0f, b.width, 12.0f, glm::vec4(1.0f, 0.8f, 0.2f, 0.9f));
            renderer.drawTextCentered("DUCKING", b.x + b.width * 0.5f, b.y - 22.0f, 1.2f, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        }

        // Mini health bar above unit
        float barW = b.width;
        float barH = 8.0f;
        renderer.drawQuad(b.x, b.y - 12.0f, barW, barH, glm::vec4(0.3f, 0.0f, 0.0f, 0.8f));
        renderer.drawQuad(b.x, b.y - 12.0f, barW * hpPercent, barH, glm::vec4(0.1f, 0.9f, 0.1f, 0.9f));
        renderer.drawBorder(b.x, b.y - 12.0f, barW, barH, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
    }

    // 3. Enemies
    for (auto* e : m_enemies) {
        if (!e || !e->alive) continue;

        Bounds b = e->getBounds();
        if (Enemy* character = dynamic_cast<Enemy*>(e)) {
            character->draw();
            const float hpPercent = character->healthFraction();
            renderer.drawQuad(b.x, b.y - 12.0f, b.width, 8.0f, glm::vec4(0.3f, 0.0f, 0.0f, 0.8f));
            renderer.drawQuad(b.x, b.y - 12.0f, b.width * hpPercent, 8.0f, glm::vec4(0.9f, 0.1f, 0.1f, 0.9f));
            renderer.drawBorder(b.x, b.y - 12.0f, b.width, 8.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
            continue;
        }

        glm::vec4 color(0.9f, 0.5f, 0.2f, 1.0f); // Easy (orange)
        std::string enemyName = "EASY";
        if (e->type == EntityType::ENEMY_MEDIUM) {
            color = glm::vec4(0.85f, 0.35f, 0.1f, 1.0f); // Medium
            enemyName = "MEDIUM";
        }
        if (e->type == EntityType::ENEMY_HARD) {
            color = glm::vec4(0.95f, 0.1f, 0.1f, 1.0f);   // Hard (dark red)
            enemyName = "HARD";
        }

        float hpPercent = (float)e->health / (float)e->maxHealth;
        glm::vec4 tint(1.0f, hpPercent, hpPercent, 1.0f);
        renderer.setTint(tint);
        renderer.drawQuad(b.x, b.y, b.width, b.height, color);
        renderer.resetShaderState();

        // Label enemy name inside body
        renderer.drawTextCentered(enemyName, b.x + b.width * 0.5f, b.y + b.height * 0.5f, 1.5f, glm::vec4(1.0f));

        // Health bar
        float barW = b.width;
        float barH = 8.0f;
        renderer.drawQuad(b.x, b.y - 12.0f, barW, barH, glm::vec4(0.3f, 0.0f, 0.0f, 0.8f));
        renderer.drawQuad(b.x, b.y - 12.0f, barW * hpPercent, barH, glm::vec4(0.9f, 0.1f, 0.1f, 0.9f));
        renderer.drawBorder(b.x, b.y - 12.0f, barW, barH, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
    }

    // 4. Projectiles
    for (auto* proj : m_projectiles) {
        if (!proj || !proj->alive) continue;
        if (Projectile* characterProjectile = dynamic_cast<Projectile*>(proj)) {
            characterProjectile->draw();
            continue;
        }
        Bounds b = proj->getBounds();
        glm::vec4 color(0.2f, 0.9f, 1.0f, 1.0f);
        std::string pName = "->";
        if (proj->type == EntityType::PROJECTILE_HIGH) {
            color = glm::vec4(1.0f, 0.9f, 0.1f, 1.0f);
            pName = "HIGH";
        }
        if (proj->type == EntityType::PROJECTILE_LOW) {
            color = glm::vec4(1.0f, 0.3f, 0.8f, 1.0f);
            pName = "LOW";
        }

        renderer.drawQuad(b.x, b.y, b.width, b.height, color);
        renderer.drawTextCentered(pName, b.x + b.width * 0.5f, b.y + b.height * 0.5f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
}

void Game::renderHUD() {
    Renderer& renderer = Renderer::getInstance();

    // Top Bar background
    renderer.drawQuad(0.0f, 0.0f, (float)Constants::SCREEN_WIDTH, Constants::TOP_BAR_HEIGHT, glm::vec4(0.09f, 0.11f, 0.14f, 1.0f));
    renderer.drawQuad(0.0f, Constants::TOP_BAR_HEIGHT - 2.0f, (float)Constants::SCREEN_WIDTH, 2.0f, glm::vec4(0.3f, 0.3f, 0.35f, 1.0f));

    // Coins counter box
    renderer.drawQuad(20.0f, 15.0f, 200.0f, 65.0f, glm::vec4(0.16f, 0.18f, 0.22f, 1.0f));
    renderer.drawBorder(20.0f, 15.0f, 200.0f, 65.0f, 2.0f, glm::vec4(1.0f, 0.84f, 0.0f, 0.85f));
    renderer.drawQuadCentered(45.0f, 47.0f, 24.0f, 24.0f, glm::vec4(1.0f, 0.84f, 0.0f, 1.0f));
    renderer.drawText("COINS: " + std::to_string(m_coins), 70.0f, 40.0f, 2.2f, glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));

    // Player Cards: Melee (Cost 10), Ranged (Cost 20), Defense (Cost 30)
    struct CardDisplay {
        CardType type;
        float x;
        std::string name;
        int cost;
        std::string hotkey;
        glm::vec4 color;
    } cards[] = {
        { CardType::MELEE,   250.0f, "MELEE",   Constants::COST_MELEE,   "[1]", glm::vec4(0.2f, 0.5f, 0.9f, 1.0f) },
        { CardType::RANGED,  380.0f, "RANGED",  Constants::COST_RANGED,  "[2]", glm::vec4(0.2f, 0.8f, 0.3f, 1.0f) },
        { CardType::DEFENSE, 510.0f, "DEFENSE", Constants::COST_DEFENSE, "[3]", glm::vec4(0.5f, 0.6f, 0.7f, 1.0f) }
    };

    for (const auto& card : cards) {
        bool unlocked = (m_coins >= card.cost);
        renderer.setGrayscale(!unlocked);

        // Card background
        renderer.drawQuad(card.x, 12.0f, 110.0f, 72.0f, card.color * (unlocked ? 1.0f : 0.5f));

        // Card border (highlighted if currently selected)
        if (m_selectedCard == card.type && unlocked) {
            renderer.drawBorder(card.x, 12.0f, 110.0f, 72.0f, 4.0f, glm::vec4(1.0f, 1.0f, 0.2f, 1.0f));
        } else {
            renderer.drawBorder(card.x, 12.0f, 110.0f, 72.0f, 2.0f, glm::vec4(0.4f, 0.4f, 0.45f, 0.8f));
        }

        // Card labels
        renderer.drawTextCentered(card.name, card.x + 55.0f, 28.0f, 1.8f, glm::vec4(1.0f));
        renderer.drawTextCentered("COST: " + std::to_string(card.cost), card.x + 55.0f, 48.0f, 1.5f, glm::vec4(1.0f, 0.9f, 0.3f, 1.0f));
        renderer.drawTextCentered(card.hotkey, card.x + 55.0f, 66.0f, 1.3f, glm::vec4(0.9f, 0.9f, 0.9f, 0.8f));

        renderer.resetShaderState();
    }

    // Controls hint text in center-right
    renderer.drawText("[P]: +50 COINS (DEMO CHEAT)  |  W: JUMP  |  S: DUCK  |  ESC: PAUSE", 640.0f, 42.0f, 1.6f, glm::vec4(0.4f, 1.0f, 0.6f, 1.0f));

    // Wave status box at top-right
    renderer.drawQuad(Constants::SCREEN_WIDTH - 250.0f, 15.0f, 230.0f, 65.0f, glm::vec4(0.16f, 0.18f, 0.22f, 1.0f));
    renderer.drawBorder(Constants::SCREEN_WIDTH - 250.0f, 15.0f, 230.0f, 65.0f, 2.0f, glm::vec4(0.4f, 0.6f, 0.8f, 0.85f));
    renderer.drawText("WAVE: " + std::to_string(m_waveManager.getCurrentWave()) + " / 3", Constants::SCREEN_WIDTH - 235.0f, 28.0f, 2.0f, glm::vec4(0.4f, 0.8f, 1.0f, 1.0f));
    renderer.drawText("ENEMIES: " + std::to_string(m_waveManager.getEnemiesRemaining()), Constants::SCREEN_WIDTH - 235.0f, 54.0f, 1.8f, glm::vec4(1.0f, 0.5f, 0.4f, 1.0f));
}

void Game::renderOverlay() {
    Renderer& renderer = Renderer::getInstance();
    glm::vec2 mousePos = Input::getMouseWorldPos();

    if (m_state == GameState::PAUSED) {
        // Dark semi-transparent dim
        renderer.drawQuad(0.0f, 0.0f, (float)Constants::SCREEN_WIDTH, (float)Constants::SCREEN_HEIGHT, glm::vec4(0.0f, 0.0f, 0.0f, 0.70f));

        // Pause Menu Dialog
        float panelW = 440.0f;
        float panelH = 340.0f;
        float panelX = (Constants::SCREEN_WIDTH - panelW) * 0.5f;
        float panelY = (Constants::SCREEN_HEIGHT - panelH) * 0.5f;

        renderer.drawQuad(panelX, panelY, panelW, panelH, glm::vec4(0.12f, 0.15f, 0.19f, 0.98f));
        renderer.drawBorder(panelX, panelY, panelW, panelH, 3.0f, glm::vec4(0.9f, 0.85f, 0.2f, 0.95f));

        renderer.drawTextCentered("GAME PAUSED", Constants::SCREEN_WIDTH * 0.5f, panelY + 35.0f, 3.2f, glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));

        SimpleButton btnResume { panelX + 70.0f, panelY + 80.0f,  300.0f, 45.0f, "RESUME",    glm::vec4(0.2f, 0.4f, 0.7f, 1.0f), glm::vec4(0.3f, 0.55f, 0.9f, 1.0f) };
        SimpleButton btnRestart{ panelX + 70.0f, panelY + 140.0f, 300.0f, 45.0f, "RESTART",   glm::vec4(0.7f, 0.45f, 0.15f, 1.0f), glm::vec4(0.9f, 0.6f, 0.2f, 1.0f) };
        SimpleButton btnMain   { panelX + 70.0f, panelY + 200.0f, 300.0f, 45.0f, "MAIN MENU", glm::vec4(0.3f, 0.35f, 0.4f, 1.0f), glm::vec4(0.4f, 0.45f, 0.55f, 1.0f) };
        SimpleButton btnExit   { panelX + 70.0f, panelY + 260.0f, 300.0f, 45.0f, "EXIT",      glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        btnResume.draw(renderer, mousePos);
        btnRestart.draw(renderer, mousePos);
        btnMain.draw(renderer, mousePos);
        btnExit.draw(renderer, mousePos);

    } else if (m_state == GameState::WON) {
        renderer.drawQuad(0.0f, 0.0f, (float)Constants::SCREEN_WIDTH, (float)Constants::SCREEN_HEIGHT, glm::vec4(0.0f, 0.0f, 0.0f, 0.75f));

        float panelW = 540.0f;
        float panelH = 340.0f;
        float panelX = (Constants::SCREEN_WIDTH - panelW) * 0.5f;
        float panelY = (Constants::SCREEN_HEIGHT - panelH) * 0.5f;

        renderer.drawQuad(panelX, panelY, panelW, panelH, glm::vec4(0.10f, 0.28f, 0.14f, 0.98f));
        renderer.drawBorder(panelX, panelY, panelW, panelH, 4.0f, glm::vec4(0.3f, 1.0f, 0.3f, 1.0f));

        renderer.drawTextCentered("VICTORY!", Constants::SCREEN_WIDTH * 0.5f, panelY + 40.0f, 4.0f, glm::vec4(1.0f, 1.0f, 0.2f, 1.0f));
        renderer.drawTextCentered("ALL 3 WAVES DEFATED! TEMPLE IS SAFE!", Constants::SCREEN_WIDTH * 0.5f, panelY + 85.0f, 2.0f, glm::vec4(0.9f, 1.0f, 0.9f, 1.0f));

        SimpleButton btnRestart{ panelX + 120.0f, panelY + 130.0f, 300.0f, 50.0f, "PLAY AGAIN", glm::vec4(0.15f, 0.55f, 0.25f, 1.0f), glm::vec4(0.2f, 0.75f, 0.35f, 1.0f) };
        SimpleButton btnMain   { panelX + 120.0f, panelY + 195.0f, 300.0f, 50.0f, "MAIN MENU",  glm::vec4(0.3f, 0.35f, 0.4f, 1.0f), glm::vec4(0.4f, 0.45f, 0.55f, 1.0f) };
        SimpleButton btnExit   { panelX + 120.0f, panelY + 260.0f, 300.0f, 50.0f, "EXIT",       glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        btnRestart.draw(renderer, mousePos);
        btnMain.draw(renderer, mousePos);
        btnExit.draw(renderer, mousePos);

    } else if (m_state == GameState::LOST) {
        renderer.drawQuad(0.0f, 0.0f, (float)Constants::SCREEN_WIDTH, (float)Constants::SCREEN_HEIGHT, glm::vec4(0.0f, 0.0f, 0.0f, 0.75f));

        float panelW = 540.0f;
        float panelH = 340.0f;
        float panelX = (Constants::SCREEN_WIDTH - panelW) * 0.5f;
        float panelY = (Constants::SCREEN_HEIGHT - panelH) * 0.5f;

        renderer.drawQuad(panelX, panelY, panelW, panelH, glm::vec4(0.32f, 0.10f, 0.10f, 0.98f));
        renderer.drawBorder(panelX, panelY, panelW, panelH, 4.0f, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));

        renderer.drawTextCentered("GAME OVER!", Constants::SCREEN_WIDTH * 0.5f, panelY + 40.0f, 4.0f, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f));
        renderer.drawTextCentered("ENEMIES BREACHED THE HOME BASE!", Constants::SCREEN_WIDTH * 0.5f, panelY + 85.0f, 2.0f, glm::vec4(1.0f, 0.8f, 0.8f, 1.0f));

        SimpleButton btnRestart{ panelX + 120.0f, panelY + 130.0f, 300.0f, 50.0f, "TRY AGAIN", glm::vec4(0.7f, 0.45f, 0.15f, 1.0f), glm::vec4(0.9f, 0.6f, 0.2f, 1.0f) };
        SimpleButton btnMain   { panelX + 120.0f, panelY + 195.0f, 300.0f, 50.0f, "MAIN MENU", glm::vec4(0.3f, 0.35f, 0.4f, 1.0f), glm::vec4(0.4f, 0.45f, 0.55f, 1.0f) };
        SimpleButton btnExit   { panelX + 120.0f, panelY + 260.0f, 300.0f, 50.0f, "EXIT",      glm::vec4(0.55f, 0.15f, 0.15f, 1.0f), glm::vec4(0.75f, 0.2f, 0.2f, 1.0f) };

        btnRestart.draw(renderer, mousePos);
        btnMain.draw(renderer, mousePos);
        btnExit.draw(renderer, mousePos);
    }
}

void Game::run() {
    float lastFrameTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(m_window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // Cap dt to avoid huge steps when dragging window
        if (dt > 0.1f) dt = 0.1f;

        glfwPollEvents();
        processInput();
        update(dt);
        render();

        glfwSwapBuffers(m_window);
        Input::update();
    }
}
