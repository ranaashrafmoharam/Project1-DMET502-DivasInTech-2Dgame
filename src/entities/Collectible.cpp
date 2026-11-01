#include "Collectible.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include "../ui/TextRenderer.h"
#include <cmath>

Collectible::Collectible(int laneIdx, float startX, float startY)
    : Entity(EntityType::COLLECTIBLE_SHARD, laneIdx, startX, startY, 1) {
    width = 36.0f;
    height = 36.0f;
    m_value = Constants::SHARD_VALUE;
}

void Collectible::update(float dt) {
    m_animTime += dt;

    if (m_collected) {
        m_floatTimer += dt;
        if (m_floatTimer >= 0.75f) {
            m_done = true;
            isAlive = false;
        }
    }
}

void Collectible::collect() {
    if (!m_collected) {
        m_collected = true;
        m_floatTimer = 0.0f;
        AudioManager::instance().playSound(SoundID::SHARD_COLLECT);
    }
}

void Collectible::draw() {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();

    if (!m_collected) {
        // Bobbing up and down (US-48)
        float bob = std::sin(m_animTime * 6.0f) * 6.0f;
        // Rotating spin gleam effect
        float rot = std::sin(m_animTime * 4.0f) * 0.25f;

        // Ground shadow beneath floating shard
        ren.drawCircle(x + width * 0.5f, y + 2.0f, 14.0f, glm::vec4(0.05f, 0.08f, 0.12f, 0.45f));

        // Draw golden shard sprite
        ren.drawSpriteCentered("shard", x + width * 0.5f, y + height * 0.5f + bob, 38.0f, 38.0f, false, rot);

        // Subtle glowing particle glints
        float glint = 0.5f + 0.5f * std::cos(m_animTime * 8.0f);
        if (glint > 0.6f) {
            ren.drawRect(x + width * 0.5f + 8.0f, y + height * 0.5f + bob + 8.0f, 3.0f, 3.0f, glm::vec4(1.0f, 1.0f, 0.8f, 0.9f));
        }
    } else {
        // Floating "+10" score pickup text rising and fading
        float rise = m_floatTimer * 45.0f;
        float alpha = glm::clamp(1.0f - (m_floatTimer / 0.75f), 0.0f, 1.0f);
        std::string scoreStr = "+" + std::to_string(m_value);
        tr.drawTextCenteredWithShadow(scoreStr, x + width * 0.5f, y + 20.0f + rise, 1.8f,
                                      glm::vec4(1.0f, 0.88f, 0.25f, alpha), glm::vec4(0.0f, 0.0f, 0.0f, alpha));
    }
}
