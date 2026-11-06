#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/Constants.h"
#include "core/Input.h"
#include "core/Game.h"
#include <string>
#include "scene/Scene.h"
#include "cutscene/Cutscene.h"
#include "scene/PropModels.h"
#include "scene/LaneHighlight.h"
#include "scene/CharacterSprites.h"
#include "ui/PlayerCards.h"
#include "ui/HUD.h"
#include "ui/HealthBar.h"

namespace
{
double mouseX = 0.0;
double mouseY = 0.0;
bool laneSelected[3] = {false, false, false};
// Member 3 preview showcases the environment after the cutscene without
// starting the gameplay combat loop. Combat remains available through the
// explicit attack control when the team wants to test it.
bool autoDemo = false;
bool forceAttack = false;
bool forceJump = false;
bool forceDuck = false;
float previewAttackCooldown = 0.0f;
int selectedPlayerCard = 0;
bool cardClicked = false;
int cardCosts[3] = {10, 20, 30};

struct DroppedShard
{
    float x;
    float y;
    int lane;
    float timer;
    bool collected;
};

struct FloatingText
{
    float x;
    float y;
    std::string text;
    float alpha;
    float r, g, b;
};

std::vector<DroppedShard> activeShards;
std::vector<FloatingText> floatTexts;
int goldenShardCount = 30;
std::string hudNotification;
float hudNotificationTime = 0.0f;
int previewEnemiesRemaining = 3;
int previewGateHealth = 100;
int playerHealth[3] = {5, 5, 5};
const int playerMaxHealth[3] = {5, 5, 5};
int enemyHealth[3] = {5, 5, 5};
const int enemyMaxHealth[3] = {5, 5, 5};

bool isCardUnlocked(int card)
{
    return card >= 0 && card < 3 && goldenShardCount >= cardCosts[card];
}

void selectCardIfUnlocked(int card)
{
    if (isCardUnlocked(card))
        selectedPlayerCard = card;
}

bool placeSelectedPlayerOnLane(int lane)
{
    if (lane < 0 || lane >= 3 || !isCardUnlocked(selectedPlayerCard))
        return false;

    PlayerCards cardLogic;
    if (!cardLogic.trySpendCard(selectedPlayerCard, goldenShardCount))
        return false;
    if (!isCardUnlocked(selectedPlayerCard))
        selectCardIfUnlocked(0);
    return true;
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        cardClicked = true;
        // Lane selection
        if (mouseY >= 390.0 && mouseY < 720.0 && mouseX >= 252.0)
        {
            int lane = static_cast<int>((mouseY - 390.0) / 110.0);
            if (lane >= 0 && lane < 3)
            {
                if (!laneSelected[lane])
                {
                    if (placeSelectedPlayerOnLane(lane))
                        laneSelected[lane] = true;
                }
                else
                {
                    laneSelected[lane] = false;
                }
            }
        }

        // Check if user clicked a dropped Golden Weapon Shard to collect it!
        for (auto& s : activeShards)
        {
            if (!s.collected)
            {
                float dx = mouseX - s.x;
                float dy = mouseY - s.y;
                if (std::sqrt(dx * dx + dy * dy) < 32.0f)
                {
                    s.collected = true;
                    goldenShardCount += 25;
                    hudNotification = "+25 GOLDEN SHARDS";
                    hudNotificationTime = 2.0f;
                    floatTexts.push_back({s.x, s.y - 10.0f, "+25 SHARDS", 1.0f, 1.0f, 0.9f, 0.2f});
                }
            }
        }
    }
#endif

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_TAB)
            autoDemo = !autoDemo;
        if (key == GLFW_KEY_A)
            forceAttack = true;
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
            forceJump = true;
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
            forceDuck = true;
        if (key == GLFW_KEY_1)
            laneSelected[0] = !laneSelected[0];
        if (key == GLFW_KEY_2)
            laneSelected[1] = !laneSelected[1];
        if (key == GLFW_KEY_3)
            laneSelected[2] = !laneSelected[2];
        if (key == GLFW_KEY_K)
            selectCardIfUnlocked(0);
        if (key == GLFW_KEY_Z)
            selectCardIfUnlocked(1);
        if (key == GLFW_KEY_C)
            selectCardIfUnlocked(2);
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        static_cast<int>(Constants::WINDOW_WIDTH),
        static_cast<int>(Constants::WINDOW_HEIGHT),
        "Ninjago: Defend the Monastery (DMET 502)",
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwShowWindow(window);
    glfwFocusWindow(window);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int fbw = 0, fbh = 0;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Input::instance().init(window);

    Scene scene;
    Cutscene cutscene;
    LaneHighlight laneHighlight;
    PropModels props;
    PlayerCards playerCards;
    HUD hud;

    scene.init();

    // Initial battlefield layout across 3 lanes
    float ninjaX[3] = {380.0f, 350.0f, 390.0f};
    float villainX[3] = {960.0f, 1020.0f, 920.0f};
    float laneY[3] = {475.0f, 585.0f, 695.0f};

    // Pre-populate some Golden Weapon Shards on the lanes for immediate gameplay showcase
    activeShards.push_back({680.0f, 475.0f, 0, 0.0f, false});
    activeShards.push_back({740.0f, 585.0f, 1, 1.2f, false});
    activeShards.push_back({620.0f, 695.0f, 2, 2.4f, false});

    auto previous = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - previous).count();
        previous = now;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            cutscene.reset();
        }

        previewAttackCooldown = std::max(0.0f, previewAttackCooldown - dt);
        if (forceAttack && previewAttackCooldown <= 0.0f)
        {
            bool attacked = false;
            for (int lane = 0; lane < 3; ++lane)
            {
                if (laneSelected[lane] && enemyHealth[lane] > 0)
                {
                    enemyHealth[lane] = std::max(0, enemyHealth[lane] - 1);
                    attacked = true;
                    break;
                }
            }
            if (attacked)
                previewAttackCooldown = 0.35f;
        }

        cutscene.update(dt);
        if (cutscene.isFinished())
        {
            playerCards.update(goldenShardCount, static_cast<float>(mouseX),
                               static_cast<float>(mouseY), cardClicked);
            selectedPlayerCard = playerCards.getSelectedCard();
        }
        cardClicked = false;

        glViewport(0, 0, 1280, 720);
        glClearColor(0.08f, 0.07f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (cutscene.isFinished())
        {
            M3Draw::advanceTime(dt);
            float curTime = M3Draw::getTime();

            // 1. Draw Environment, Monestary Base, and Lanes
            scene.draw();

            // 2. Lane Hover & Selection Highlights
            int hoveredLane = -1;
            if (mouseY >= 390.0 && mouseY < 720.0 && mouseX >= 252.0)
            {
                hoveredLane = static_cast<int>((mouseY - 390.0) / 110.0);
            }

            for (int lane = 0; lane < 3; ++lane)
            {
                bool isHovered = (lane == hoveredLane);
                bool isSel = laneSelected[lane];
                laneHighlight.draw(lane, isHovered, isSel);
            }

            // 3. Update & Draw Dropped Golden Weapon Shards on the Lanes
            for (auto& shard : activeShards)
            {
                if (!shard.collected)
                {
                    shard.timer += dt;
                    // Floating & Spinning Golden Weapon Shard
                    float bobY = shard.y - 12.0f + std::sin(shard.timer * 4.0f) * 5.0f;
                    props.drawCollectible(shard.x, bobY, curTime + shard.timer);

                    // Golden glowing aura around the shard
                    float auraPulse = 0.5f + 0.5f * std::sin(shard.timer * 6.0f);
                    M3Draw::circle(shard.x, bobY, 14.0f + auraPulse * 4.0f, 1.0f, 0.85f, 0.2f, 0.25f * auraPulse, 16);

                    // Check if Ninja stepped near the shard to collect it automatically
                    if (std::abs(ninjaX[shard.lane] - shard.x) < 30.0f)
                    {
                        shard.collected = true;
                        goldenShardCount += 25;
                        hudNotification = "+25 GOLDEN SHARDS";
                        hudNotificationTime = 2.0f;
                        floatTexts.push_back({shard.x, shard.y - 18.0f, "+25 SHARDS", 1.0f, 1.0f, 0.95f, 0.3f});
                    }
                }
            }

            // 4. Update & Render Characters on All 3 Battle Lanes
            for (int lane = 0; lane < 3; ++lane)
            {
                float yPos = laneY[lane];
                float dist = villainX[lane] - ninjaX[lane];

                CharacterSprites::State nState = CharacterSprites::State::RUN;
                CharacterSprites::State vState = CharacterSprites::State::RUN;

                if (forceAttack)
                {
                    nState = CharacterSprites::State::ATTACK;
                    vState = CharacterSprites::State::ATTACK;
                }
                else if (forceJump)
                {
                    nState = CharacterSprites::State::JUMP;
                }
                else if (forceDuck)
                {
                    nState = CharacterSprites::State::DUCK;
                }
                else if (autoDemo)
                {
                    if (dist > 160.0f)
                    {
                        ninjaX[lane] += 55.0f * dt;
                        villainX[lane] -= 50.0f * dt;
                        nState = CharacterSprites::State::RUN;
                        vState = CharacterSprites::State::RUN;
                    }
                    else
                    {
                        // In attack range: strike!
                        nState = CharacterSprites::State::ATTACK;
                        vState = CharacterSprites::State::ATTACK;

                        // Periodically spawn a dropped Golden Weapon Shard on combat hit
                        if (std::fmod(curTime, 3.2f) < dt * 1.5f && activeShards.size() < 8)
                        {
                            activeShards.push_back({villainX[lane] - 25.0f, yPos, lane, 0.0f, false});
                        }

                        // Cycle positions after battle sequence
                        if (std::fmod(curTime, 9.0f) > 8.5f)
                        {
                            ninjaX[lane] = 340.0f + lane * 30.0f;
                            villainX[lane] = 1050.0f - lane * 40.0f;
                        }
                    }
                }
                else
                {
                    nState = CharacterSprites::State::IDLE;
                    // Keep the preview non-combat when Auto-Run is off, but
                    // leave every villain in its RUN animation so the green
                    // and purple serpents visibly slither in place together.
                    vState = CharacterSprites::State::RUN;
                }

                // Render Ninja & Villain for this lane
                if (lane == 0)
                {
                    CharacterSprites::drawKai(ninjaX[lane], yPos, 2.7f, nState, curTime + lane, true);
                    CharacterSprites::drawVenomari(villainX[lane], yPos, 2.7f, vState, curTime + lane, false);
                }
                else if (lane == 1)
                {
                    CharacterSprites::drawZane(ninjaX[lane], yPos, 2.7f, nState, curTime + lane, true);
                    CharacterSprites::drawPythor(villainX[lane], yPos, 2.7f, vState, curTime + lane, false);
                }
                else
                {
                    CharacterSprites::drawCole(ninjaX[lane], yPos, 2.7f, nState, curTime + lane, true);
                    CharacterSprites::drawVenomari(villainX[lane], yPos, 2.7f, vState, curTime + lane + 1.2f, false);
                }

                // Player and enemy health bars in the Member 3 pixel-art style.
                HealthBar::draw(ninjaX[lane], yPos - 58.0f, 64.0f,
                                playerHealth[lane], playerMaxHealth[lane], false);
                HealthBar::draw(villainX[lane], yPos - 58.0f, 64.0f,
                                enemyHealth[lane], enemyMaxHealth[lane], true);
            }

            // 5. Floating Score / Shard Collection Popups
            for (auto it = floatTexts.begin(); it != floatTexts.end();)
            {
                it->y -= 25.0f * dt;
                it->alpha -= 0.8f * dt;
                if (it->alpha <= 0.0f)
                {
                    it = floatTexts.erase(it);
                }
                else
                {
                    M3Draw::text(it->x - 20.0f, it->y, it->text.c_str(), 1.3f, it->r, it->g, it->b, it->alpha);
                    ++it;
                }
            }

            // =========================================================================
            // 6. Reusable gameplay HUD
            // =========================================================================
            GameplayHudState hudState;
            hudState.coins = goldenShardCount;
            hudState.currentWave = 1;
            hudState.totalWaves = 3;
            hudState.enemiesRemaining = 0;
            for (int lane = 0; lane < 3; ++lane)
                if (enemyHealth[lane] > 0)
                    ++hudState.enemiesRemaining;
            hudState.gateHealth = previewGateHealth;
            if (hudNotificationTime > 0.0f)
            {
                hudNotificationTime = std::max(0.0f, hudNotificationTime - dt);
                hudState.notification = hudNotification;
                hudState.notificationAlpha = std::min(1.0f, hudNotificationTime);
            }
            hud.draw(hudState);

            // =========================================================================
            // 7. Player Selection Cards
            // =========================================================================
            playerCards.draw();

            // =========================================================================
            // 8. Animation & Game Controls HUD at Top-Right
            // =========================================================================
            M3Draw::quad(920.0f, 8.0f, 345.0f, 110.0f, 0.08f, 0.08f, 0.14f, 0.90f);
            M3Draw::quad(921.0f, 9.0f, 343.0f, 108.0f, 0.75f, 0.65f, 0.28f, 0.90f);
            M3Draw::quad(923.0f, 11.0f, 339.0f, 104.0f, 0.12f, 0.12f, 0.18f, 0.95f);
            M3Draw::text(932.0f, 18.0f, "CONTROLS & ANIMATIONS", 1.7f, 1.0f, 0.95f, 0.5f);
            M3Draw::text(932.0f, 42.0f, "A: Attack | W: Jump | S: Duck", 1.35f, 0.90f, 0.95f, 1.0f);
            M3Draw::text(932.0f, 62.0f, "TAB: Toggle Auto-Run | R: Cutscene", 1.35f, 0.90f, 0.95f, 1.0f);
            M3Draw::text(932.0f, 82.0f, "Click Shards to Collect | Lanes", 1.35f, 0.82f, 0.90f, 0.82f);

            M3Draw::flush();
        }
        else
        {
            // Play Intro Cutscene
            cutscene.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    if (!game.init(window)) {
        std::cerr << "Failed to initialize Game" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    game.run();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
