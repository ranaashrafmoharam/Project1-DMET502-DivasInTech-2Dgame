// ====================================================================
// FILE:     src/main.cpp
// OWNER:    Member 3 Preview Launcher
// TASKS:    M3-PREVIEW, US-06 to US-10, US-12, US-13, US-14, US-16, US-46, US-47, US-48, US-51 to US-54
//
// PURPOSE:
//   Interactive preview executable for Member 3:
//   - Full Game Boy / Pokemon style intro cutscene with exact 9 dialogues,
//     high-detail speaker portraits, and animated Ninjago pixel art characters
//   - Interactive Monastery playfield: 3 battle lanes, hover & click selection,
//     pagoda, torii gate, clan banners, lanterns, training area, cherry tree,
//     monastery gate safe zone, and animated props (weapons, projectiles, collectible)
//   - 3 Ninja (Kai, Zane, Cole) & Villains (Venomari, Pythor) with authentic
//     Aseprite / Bullet Bunny style Running & Attacking animations!
//   - Official Theme Currency: GOLDEN WEAPON SHARDS!
//     HUD counter, dropped 3D spinning/bobbing shards in battle lanes, collection popups
//   - Press R to replay cutscene anytime, Space/Esc to skip
// ====================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include "scene/Scene.h"
#include "cutscene/Cutscene.h"
#include "scene/PropModels.h"
#include "scene/LaneHighlight.h"
#include "scene/CharacterSprites.h"

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
int goldenShardCount = 175;

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Lane selection
        if (mouseY >= 390.0 && mouseY < 720.0 && mouseX >= 252.0)
        {
            int lane = static_cast<int>((mouseY - 390.0) / 110.0);
            if (lane >= 0 && lane < 3)
            {
                laneSelected[lane] = !laneSelected[lane];
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
                    floatTexts.push_back({s.x, s.y - 10.0f, "+25 SHARDS", 1.0f, 1.0f, 0.9f, 0.2f});
                }
            }
        }
    }
}

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
    }
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_A)
            forceAttack = false;
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
            forceJump = false;
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
            forceDuck = false;
    }
}

GLFWwindow* createPreviewWindow()
{
    if (glfwInit() != GLFW_TRUE)
        return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Member 3 - Ninjago Preview (Game Boy RPG Edition)", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    return window;
}
}

int main()
{
    GLFWwindow* window = createPreviewWindow();
    if (!window)
        return EXIT_FAILURE;

    Scene scene;
    Cutscene cutscene;
    LaneHighlight laneHighlight;
    PropModels props;

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

        cutscene.update(dt);

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
            // 6. OFFICIAL CURRENCY HUD: GOLDEN WEAPON SHARDS
            // =========================================================================
            float curBoxX = 20.0f;
            float curBoxY = 8.0f;
            float curBoxW = 230.0f;
            float curBoxH = 110.0f;

            M3Draw::quad(curBoxX, curBoxY, curBoxW, curBoxH, 0.08f, 0.07f, 0.14f, 0.95f);
            M3Draw::quad(curBoxX + 2, curBoxY + 2, curBoxW - 4, curBoxH - 4, 0.88f, 0.76f, 0.32f, 0.95f); // Gold rim
            M3Draw::quad(curBoxX + 5, curBoxY + 5, curBoxW - 10, curBoxH - 10, 0.12f, 0.11f, 0.18f, 0.95f);

            // Animated spinning 3D Golden Weapon Shard Icon inside the HUD
            props.drawCollectible(curBoxX + 38.0f, curBoxY + 44.0f, curTime);

            M3Draw::text(curBoxX + 72.0f, curBoxY + 22.0f, "GOLDEN SHARDS", 1.55f, 1.0f, 0.90f, 0.40f);

            char shardStr[32];
            std::snprintf(shardStr, sizeof(shardStr), "%d", goldenShardCount);
            M3Draw::text(curBoxX + 72.0f, curBoxY + 46.0f, shardStr, 2.5f, 1.0f, 0.98f, 0.65f);

            M3Draw::text(curBoxX + 16.0f, curBoxY + 88.0f, "Defeat snakes to collect!", 1.2f, 0.9f, 0.85f, 0.7f);

            // =========================================================================
            // 7. Props & Weapons Showcase Panel at Top-Center
            // =========================================================================
            M3Draw::quad(260.0f, 8.0f, 650.0f, 110.0f, 0.08f, 0.07f, 0.14f, 0.90f);
            M3Draw::quad(262.0f, 10.0f, 646.0f, 106.0f, 0.78f, 0.65f, 0.28f, 0.90f);
            M3Draw::quad(264.0f, 12.0f, 642.0f, 102.0f, 0.12f, 0.11f, 0.18f, 0.95f);
            M3Draw::text(280.0f, 18.0f, "PROPS & WEAPONS PREVIEW (MEMBER 3)", 1.75f, 1.0f, 0.95f, 0.5f);

            // 1. Sword of Fire (Melee)
            props.drawMeleeWeapon(325.0f, 65.0f, -0.65f);
            M3Draw::text(302.0f, 94.0f, "FIRE SWORD", 1.3f, 1.0f, 0.58f, 0.25f);

            // 2. Frost Bow & Shurikens (Ranged)
            props.drawRangedWeapon(440.0f, 65.0f, -0.65f);
            M3Draw::text(416.0f, 94.0f, "FROST BOW", 1.3f, 0.5f, 0.90f, 1.0f);

            // 3. Projectiles (Zane Shuriken, Venomari Acid, Hypnobrai Wave)
            props.drawProjectile(0, 535.0f, 62.0f, 0.0f);
            M3Draw::text(516.0f, 94.0f, "SHURIKEN", 1.3f, 0.7f, 0.92f, 1.0f);

            props.drawProjectile(1, 625.0f, 62.0f, 0.0f);
            M3Draw::text(606.0f, 94.0f, "VENOM SPIT", 1.3f, 0.45f, 0.95f, 0.35f);

            props.drawProjectile(2, 725.0f, 62.0f, 0.0f);
            M3Draw::text(700.0f, 94.0f, "HYPNO WAVE", 1.3f, 0.85f, 0.45f, 0.95f);

            // 4. Golden Weapon Shard Collectible (Spinning 3D & Bobbing)
            props.drawCollectible(835.0f, 58.0f, curTime);
            M3Draw::text(796.0f, 94.0f, "GOLDEN SHARD", 1.3f, 1.0f, 0.90f, 0.25f);

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

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
