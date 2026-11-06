#include "Cutscene.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include "../ui/TextRenderer.h"
#include <cmath>
#include <algorithm>

Cutscene& Cutscene::instance() {
    static Cutscene cs;
    return cs;
}

void Cutscene::reset() {
    m_timeline = 0.0f;
    m_finished = false;

    m_wuX = 220.0f;
    m_kaiX = -120.0f;
    m_garmadonX = 1380.0f;
    m_venomariX = 1420.0f;

    m_acidActive = false;
    m_slashActive = false;
}

void Cutscene::skip() {
    m_finished = true;
}

void Cutscene::update(float dt) {
    if (m_finished) return;
    m_timeline += dt;

    if (m_timeline >= Constants::CUTSCENE_DURATION) {
        m_finished = true;
        return;
    }

    // Beat 2: Garmadon and Venomari advance in from right (3.8s .. 8.0s)
    if (m_timeline >= 3.8f && m_timeline < 8.0f) {
        float t = (m_timeline - 3.8f) / 4.2f;
        m_garmadonX = glm::mix(1380.0f, 960.0f, std::min(1.0f, t * 1.2f));
        m_venomariX = glm::mix(1440.0f, 820.0f, std::min(1.0f, t * 1.1f));
    }

    // Beat 3: Kai rushes in from left (8.2s .. 12.0s)
    if (m_timeline >= 8.2f && m_timeline < 12.0f) {
        float t = (m_timeline - 8.2f) / 3.8f;
        m_kaiX = glm::mix(-120.0f, 520.0f, std::min(1.0f, t * 1.4f));
    }

    // Beat 4: Combat interaction - venom spit & sword parry (12.8s .. 16.0s)
    if (m_timeline >= 12.8f && m_timeline < 16.0f) {
        float t = (m_timeline - 12.8f);
        m_acidActive = true;
        m_acidX = glm::mix(820.0f, 580.0f, std::min(1.0f, t * 1.5f));
        m_acidY = 240.0f + std::sin(t * 3.14159f) * 45.0f;

        if (t > 0.65f) {
            m_slashActive = true;
            m_slashRot = (t - 0.65f) * 12.0f;
        }
    } else {
        m_acidActive = false;
        m_slashActive = false;
    }
}

void Cutscene::draw() {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();
    const float W = Constants::WINDOW_WIDTH;
    const float H = Constants::WINDOW_HEIGHT;

    // 1. Scene Background: Dusk mountain sky
    ren.drawRect(0, 0, W, H, glm::vec4(0.08f, 0.08f, 0.16f, 1.0f));

    // Sunset gradient haze behind monastery
    for (int i = 0; i < 8; ++i) {
        float y = 200.0f + i * 40.0f;
        float alpha = 0.25f * (1.0f - (float)i / 8.0f);
        ren.drawRect(0, y, W, 40.0f, glm::vec4(0.85f, 0.35f, 0.20f, alpha));
    }

    // Distant mountain peaks
    for (int i = 0; i < 6; ++i) {
        float mx = i * 240.0f - 60.0f;
        ren.drawTri(glm::vec2(mx, 200.0f), glm::vec2(mx + 280.0f, 200.0f), glm::vec2(mx + 140.0f, 440.0f),
                    glm::vec4(0.12f, 0.14f, 0.24f, 1.0f));
    }

    // Monastery stone courtyard ground
    ren.drawRect(0, 0, W, 220.0f, glm::vec4(0.18f, 0.20f, 0.25f, 1.0f));
    ren.drawRect(0, 218.0f, W, 3.0f, glm::vec4(0.35f, 0.40f, 0.50f, 1.0f));

    // Monastery Gate on the left (Home Base)
    ren.drawSprite("monastery_gate", 0.0f, 180.0f, 160.0f, 400.0f);

    // Environment decorations: Cherry tree, stone lantern, banners
    ren.drawSprite("cherry_tree", 30.0f, 190.0f, 140.0f, 140.0f);
    ren.drawSprite("stone_lantern", 180.0f, 190.0f, 50.0f, 70.0f);
    ren.drawSprite("dragon_banner", 290.0f, 190.0f, 45.0f, 85.0f);
    ren.drawSprite("stone_lantern", W - 140.0f, 190.0f, 50.0f, 70.0f);

    // Golden Weapons on the altar (what the players defend! US-09)
    float altarX = 220.0f, altarY = 210.0f;
    ren.drawRect(altarX - 10, altarY, 80, 20, glm::vec4(0.50f, 0.25f, 0.15f, 1.0f));
    ren.drawRect(altarX - 6, altarY + 18, 72, 4, glm::vec4(0.90f, 0.75f, 0.20f, 1.0f));
    // 4 glowing golden shards representing the 4 Golden Weapons
    for (int w = 0; w < 4; ++w) {
        float bob = std::sin(m_timeline * 4.0f + w) * 3.0f;
        ren.drawSpriteCentered("shard", altarX + w * 18.0f + 5.0f, altarY + 32.0f + bob, 22.0f, 22.0f);
    }

    // 2. Animated Characters (US-07: At least one player and one enemy interacting)
    // Sensei Wu standing by the altar guarding the weapons
    ren.drawSprite("wu", m_wuX, 205.0f, 110.0f, 125.0f);

    // Kai (Fire Ninja) running / posing
    if (m_kaiX > -100.0f) {
        ren.drawSprite("kai", m_kaiX, 205.0f, 110.0f, 145.0f);
        // Kai's katana drawn
        ren.drawSpriteCentered("sword", m_kaiX + 90.0f, 255.0f, 40.0f, 40.0f, false, m_slashActive ? m_slashRot : 0.4f);
    }

    // Venomari (Enemy)
    if (m_venomariX < W + 100.0f) {
        ren.drawSprite("venomari", m_venomariX, 205.0f, 110.0f, 145.0f, true);
    }

    // Lord Garmadon (Main Villain)
    if (m_garmadonX < W + 100.0f) {
        ren.drawSprite("garmadon", m_garmadonX, 205.0f, 130.0f, 170.0f, true);
    }

    // Combat interaction projectiles (Beat 4)
    if (m_acidActive) {
        ren.drawSpriteCentered("acid", m_acidX, m_acidY, 32.0f, 32.0f);
    }
    if (m_slashActive) {
        // Fire burst parry effect
        ren.drawCircle(580.0f, 260.0f, 28.0f, glm::vec4(1.0f, 0.50f, 0.10f, 0.75f));
        ren.drawCircle(580.0f, 260.0f, 16.0f, glm::vec4(1.0f, 0.90f, 0.20f, 0.90f));
    }

    // 3. Subtitles & Dialogue Box (US-08)
    std::string speaker;
    std::string line1, line2;

    if (m_timeline < 3.8f) {
        speaker = "SENSEI WU";
        line1 = "For generations, the Monastery has guarded the Golden Weapons.";
        line2 = "Whoever wields them commands the power to rule all of Ninjago!";
    } else if (m_timeline < 8.2f) {
        speaker = "LORD GARMADON";
        line1 = "Hand over the Golden Weapons, brother! The Serpentine have";
        line2 = "risen from the depths to take what is rightfully ours!";
    } else if (m_timeline < 12.8f) {
        speaker = "KAI (FIRE NINJA)";
        line1 = "Not while the Ninja stand guard! Spinjitzu will burn";
        line2 = "any snake that dares set foot past these monastery gates!";
    } else if (m_timeline < 16.2f) {
        speaker = "VENOMARI GENERAL";
        line1 = "Sssss! Spit your venom and swarm every battle line!";
        line2 = "Leave nothing standing! Forward, Serpentine army!";
    } else {
        speaker = "SENSEI WU";
        line1 = "Ninja, take your battle positions across all lanes!";
        line2 = "Defend the Monastery Gate! BATTLE COMMENCE!";
    }

    // Classic Pokemon Dialogue Box at bottom
    float boxW = W - 120.0f;
    float boxH = 120.0f;
    float boxX = 60.0f;
    float boxY = 25.0f;
    tr.drawDialogueBox(speaker, line1, line2, boxX, boxY, boxW, boxH, 2.2f);

    // Skip helper button / hint (US-10)
    tr.drawTextWithShadow("[SPACE / ESC] SKIP INTRO", W - 280.0f, H - 28.0f, 1.6f,
                          glm::vec4(0.85f, 0.85f, 0.90f, 0.9f));

    // Timeline progress bar at very top
    float progress = glm::clamp(m_timeline / Constants::CUTSCENE_DURATION, 0.0f, 1.0f);
    ren.drawRect(0, H - 4.0f, W, 4.0f, glm::vec4(0.2f, 0.2f, 0.3f, 1.0f));
    ren.drawRect(0, H - 4.0f, W * progress, 4.0f, glm::vec4(0.95f, 0.75f, 0.20f, 1.0f));
}
