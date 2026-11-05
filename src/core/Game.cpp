#include "Game.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Constants.h"
#include "Input.h"
#include "Renderer.h"
#include "Texture.h"
#include "../audio/AudioManager.h"
#include "../ui/TextRenderer.h"
#include "../ui/Menus.h"
#include "../cutscene/Cutscene.h"
#include "../world/LaneGrid.h"
#include "../world/WaveManager.h"
#include "../world/CollisionSystem.h"
#include "../scene/Scene.h"
#include "../scene/LaneHighlight.h"
#include <iostream>
#include <fstream>

bool Game::saveScreenshotBMP(const std::string& path, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 4);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());

    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    uint16_t fileType = 0x4D42; // "BM"
    uint32_t fileSize = 54 + width * height * 4;
    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;
    uint32_t offset = 54;

    out.write(reinterpret_cast<const char*>(&fileType), 2);
    out.write(reinterpret_cast<const char*>(&fileSize), 4);
    out.write(reinterpret_cast<const char*>(&reserved1), 2);
    out.write(reinterpret_cast<const char*>(&reserved2), 2);
    out.write(reinterpret_cast<const char*>(&offset), 4);

    uint32_t headerSize = 40;
    int32_t biWidth = width;
    int32_t biHeight = height;
    uint16_t planes = 1;
    uint16_t bitCount = 32;
    uint32_t compression = 0;
    uint32_t imageSize = width * height * 4;
    int32_t xPels = 2835;
    int32_t yPels = 2835;
    uint32_t clrUsed = 0;
    uint32_t clrImportant = 0;

    out.write(reinterpret_cast<const char*>(&headerSize), 4);
    out.write(reinterpret_cast<const char*>(&biWidth), 4);
    out.write(reinterpret_cast<const char*>(&biHeight), 4);
    out.write(reinterpret_cast<const char*>(&planes), 2);
    out.write(reinterpret_cast<const char*>(&bitCount), 2);
    out.write(reinterpret_cast<const char*>(&compression), 4);
    out.write(reinterpret_cast<const char*>(&imageSize), 4);
    out.write(reinterpret_cast<const char*>(&xPels), 4);
    out.write(reinterpret_cast<const char*>(&yPels), 4);
    out.write(reinterpret_cast<const char*>(&clrUsed), 4);
    out.write(reinterpret_cast<const char*>(&clrImportant), 4);

    out.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    return true;
}

bool Game::init(GLFWwindow* window) {
    m_window = window;

    // Initialize all core and UI sub-systems
    if (!Renderer::instance().init()) return false;
    TextureManager::instance().init();
    TextRenderer::instance().init();
    AudioManager::instance().init();
    Menus::instance().init();
    LaneGrid::instance().init();
    Scene::instance().init();

    reset();
    return true;
}

void Game::setState(GameState s) {
    m_state = s;
}

void Game::startCutscene() {
    Cutscene::instance().reset();
    m_state = GameState::CUTSCENE;
}

void Game::reset() {
    // US-04: Restart clears all entities, coins, waves, unlocked cards and lane selections, restores starting coins
    m_coins = Constants::STARTING_COINS; // 30
    m_players.clear();
    m_enemies.clear();
    m_projectiles.clear();

    LaneGrid::instance().reset();
    WaveManager::instance().reset();
    CollisionSystem::instance().reset();
    Cutscene::instance().reset();
}

void Game::quit() {
    if (m_window) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}

void Game::run() {
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window)) {
        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - lastTime);
        if (dt > 0.1f) dt = 0.1f; // Clamp delta time to avoid spiral of death
        lastTime = currentTime;

        // Update mouse position
        int winW, winH;
        glfwGetWindowSize(m_window, &winW, &winH);
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);
        Input::instance().updateCursor(mx, my, winW, winH);

        // Update game logic
        update(dt);

        // Render frame
        render();

        if (m_autoTest) {
            m_autoTestFrame++;
            int w = static_cast<int>(Constants::WINDOW_WIDTH);
            int h = static_cast<int>(Constants::WINDOW_HEIGHT);

            if (m_autoTestFrame == 2) {
                saveScreenshotBMP("menu.bmp", w, h);
                setState(GameState::SOUND_SETTINGS);
            } else if (m_autoTestFrame == 4) {
                saveScreenshotBMP("sound_settings.bmp", w, h);
                startCutscene();
                Cutscene::instance().update(6.0f);
            } else if (m_autoTestFrame == 6) {
                saveScreenshotBMP("cutscene.bmp", w, h);
                reset();
                setState(GameState::PLAYING);

                // Spawn defenders: Kai (lane 0), Zane (lane 1), Cole (lane 2)
                m_players.push_back(std::make_shared<Player>(EntityType::PLAYER_MELEE, 0, 380.0f));
                m_players.push_back(std::make_shared<Player>(EntityType::PLAYER_RANGED, 1, 300.0f));
                m_players.push_back(std::make_shared<Player>(EntityType::PLAYER_DEFENSE, 2, 450.0f));

                // Spawn enemies: Venomari (lane 0), Hypnobrai (lane 1), Constrictai (lane 2)
                m_enemies.push_back(std::make_shared<Enemy>(EntityType::ENEMY_EASY, 0, 800.0f));
                m_enemies.push_back(std::make_shared<Enemy>(EntityType::ENEMY_MEDIUM, 1, 880.0f));
                m_enemies.push_back(std::make_shared<Enemy>(EntityType::ENEMY_HARD, 2, 700.0f));

                // Projectiles
                m_projectiles.push_back(std::make_shared<Projectile>(ProjectileType::PLAYER_SHURIKEN, 1, 420.0f, 0.0f));
                m_projectiles.push_back(std::make_shared<Projectile>(ProjectileType::ENEMY_UPPER_ACID, 0, 680.0f, 0.0f));

                // Select Lane 1
                LaneGrid::instance().selectSingleLane(1);

                // Apply damage to demonstrate red tint (US-43) and HP bars (US-44)
                m_players[0]->takeDamage(2);
                m_enemies[0]->takeDamage(3);

                m_coins = 20; // Kai & Zane unlocked, Cole locked
            } else if (m_autoTestFrame == 8) {
                saveScreenshotBMP("gameplay.bmp", w, h);
                setState(GameState::PAUSED);
            } else if (m_autoTestFrame == 10) {
                saveScreenshotBMP("pause.bmp", w, h);
                setState(GameState::WON);
            } else if (m_autoTestFrame == 12) {
                saveScreenshotBMP("victory.bmp", w, h);
                setState(GameState::LOST);
            } else if (m_autoTestFrame == 14) {
                saveScreenshotBMP("defeat.bmp", w, h);
                quit();
            }
        }

        glfwSwapBuffers(m_window);

        // Reset per-frame input triggers
        Input::instance().newFrame();
        glfwPollEvents();
    }
}

void Game::update(float dt) {
    glm::vec2 mouse = Input::instance().getMouseWorldPos();
    bool leftClicked = Input::instance().wasLeftClicked();

    switch (m_state) {
    case GameState::MAIN_MENU:
        Menus::instance().updateMain(*this, mouse, leftClicked);
        break;

    case GameState::SOUND_SETTINGS:
        Menus::instance().updateSound(*this, mouse, leftClicked);
        break;

    case GameState::CUTSCENE:
        // Skip with Esc, Space or Enter (US-10)
        if (Input::instance().escPressed() || Input::instance().spacePressed() || Input::instance().enterPressed()) {
            Cutscene::instance().skip();
        }
        Cutscene::instance().update(dt);
        if (Cutscene::instance().isFinished()) {
            reset();
            m_state = GameState::PLAYING;
        }
        break;

    case GameState::PLAYING: {
        // Esc opens Pause menu (US-02)
        if (Input::instance().escPressed()) {
            m_state = GameState::PAUSED;
            AudioManager::instance().playSound(SoundID::LANE_SELECT);
            return;
        }

        // Update HUD coin count
        // Lane grid hover & selection
        int hoveredLane = LaneGrid::instance().laneAt(mouse);

        if (leftClicked && hoveredLane != -1) {
            LaneGrid::instance().toggleLane(hoveredLane);
        }

        // Jump & Duck commands across all selected lanes (US-29, US-30)
        if (Input::instance().jumpPressed()) {
            std::vector<int> selLanes = LaneGrid::instance().getSelectedLanes();
            for (auto& p : m_players) {
                if (!p || !p->isAlive) continue;
                for (int l : selLanes) {
                    if (p->lane == l) p->jump();
                }
            }
        }

        if (Input::instance().duckPressed()) {
            std::vector<int> selLanes = LaneGrid::instance().getSelectedLanes();
            for (auto& p : m_players) {
                if (!p || !p->isAlive) continue;
                for (int l : selLanes) {
                    if (p->lane == l) p->duck();
                }
            }
        }

        // Scene environment ambient animation
        Scene::instance().update(dt);

        // Waves & Spawning (US-39)
        WaveManager::instance().update(dt, m_enemies);

        // Update players
        for (auto& p : m_players) {
            if (p && p->isAlive) {
                p->update(dt);
                // Ranged player shoots ice shuriken when ready
                if (p->readyToShoot) {
                    p->readyToShoot = false;
                    p->resetAttackCooldown();
                    m_projectiles.push_back(std::make_shared<Projectile>(
                        ProjectileType::PLAYER_SHURIKEN, p->lane, p->x + p->width + 5.0f, p->y));
                    AudioManager::instance().playSound(SoundID::ATTACK_SHURIKEN);
                }
            }
        }

        // Update enemies
        for (auto& e : m_enemies) {
            if (e && e->isAlive) {
                e->update(dt);
                // Ranged enemies fire high or low attacks
                if (e->readyToShoot) {
                    e->readyToShoot = false;
                    e->resetAttackCooldown();
                    if (e->type == EntityType::ENEMY_EASY) {
                        // Upper ranged attack (duck to dodge!)
                        m_projectiles.push_back(std::make_shared<Projectile>(
                            ProjectileType::ENEMY_UPPER_ACID, e->lane, e->x - 15.0f, e->y));
                        AudioManager::instance().playSound(SoundID::ATTACK_HISS);
                    } else if (e->type == EntityType::ENEMY_MEDIUM) {
                        // Lower ranged attack (jump to dodge!)
                        m_projectiles.push_back(std::make_shared<Projectile>(
                            ProjectileType::ENEMY_LOWER_SONIC, e->lane, e->x - 15.0f, e->y));
                        AudioManager::instance().playSound(SoundID::ATTACK_HISS);
                    }
                }
            }
        }

        // Update projectiles
        for (auto& pr : m_projectiles) {
            if (pr && pr->isAlive) pr->update(dt);
        }

        // Collisions, attacks, pickups, and base breaches
        CollisionSystem::instance().update(m_players, m_enemies, m_projectiles, m_coins);

        // Win / Lose conditions:
        // US-50: If enemy reaches home base in any lane, lose!
        if (CollisionSystem::instance().hasEnemyReachedBase()) {
            m_state = GameState::LOST;
            AudioManager::instance().playSound(SoundID::DEFEAT);
        }
        // US-49: Defeat all enemies in all three waves -> win!
        else if (WaveManager::instance().allWavesFinished()) {
            m_state = GameState::WON;
            AudioManager::instance().playSound(SoundID::VICTORY);
        }
        break;
    }

    case GameState::PAUSED:
        // Esc continues game
        if (Input::instance().escPressed()) {
            m_state = GameState::PLAYING;
            AudioManager::instance().playSound(SoundID::LANE_SELECT);
            return;
        }
        // Menu updates, game state stays frozen underneath (US-02)
        Menus::instance().updatePause(*this, mouse, leftClicked);
        break;

    case GameState::WON:
        Menus::instance().updateEnd(*this, mouse, leftClicked, true);
        break;

    case GameState::LOST:
        Menus::instance().updateEnd(*this, mouse, leftClicked, false);
        break;
    }
}

void Game::render() {
    glClearColor(0.04f, 0.06f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Renderer::instance().beginFrame();

    switch (m_state) {
    case GameState::MAIN_MENU:
        Menus::instance().drawMain();
        break;

    case GameState::SOUND_SETTINGS:
        Menus::instance().drawSound();
        break;

    case GameState::CUTSCENE:
        Cutscene::instance().draw();
        break;

    case GameState::PLAYING:
    case GameState::PAUSED:
    case GameState::WON:
    case GameState::LOST: {
        // Draw gameplay world
        Scene::instance().draw();
        LaneHighlight::draw();

        // Defenders
        for (auto& p : m_players) {
            if (p && p->isAlive) p->draw();
        }

        // Enemies
        for (auto& e : m_enemies) {
            if (e && e->isAlive) e->draw();
        }

        // Projectiles
        for (auto& pr : m_projectiles) {
            if (pr && pr->isAlive) pr->draw();
        }

        // Overlays for Pause, Win, or Lose
        if (m_state == GameState::PAUSED) {
            Menus::instance().drawPause();
        } else if (m_state == GameState::WON) {
            Menus::instance().drawEnd(true);
        } else if (m_state == GameState::LOST) {
            Menus::instance().drawEnd(false);
        }
        break;
    }
    }
}
