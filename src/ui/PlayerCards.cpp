#include "PlayerCards.h"
#include "../core/Renderer.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include "TextRenderer.h"
#include <cmath>

PlayerCards& PlayerCards::instance() {
    static PlayerCards pc;
    return pc;
}

void PlayerCards::init() {
    m_cards[0].id = 0;
    m_cards[0].name = "KAI";
    m_cards[0].sprite = "kai";
    m_cards[0].cost = Constants::COST_MELEE; // 10
    m_cards[0].desc = "MELEE 5HP";

    m_cards[1].id = 1;
    m_cards[1].name = "ZANE";
    m_cards[1].sprite = "zane";
    m_cards[1].cost = Constants::COST_RANGED; // 20
    m_cards[1].desc = "RANGED 5HP";

    m_cards[2].id = 2;
    m_cards[2].name = "COLE";
    m_cards[2].sprite = "cole";
    m_cards[2].cost = Constants::COST_DEFENSE; // 30
    m_cards[2].desc = "WALL 7HP";

    // Position cards horizontally at top right
    const float startX = 220.0f;
    const float cardY = Constants::WINDOW_HEIGHT - 65.0f;
    const float cardW = 145.0f;
    const float cardH = 58.0f;
    const float spacing = 15.0f;

    for (int i = 0; i < 3; ++i) {
        m_cards[i].x = startX + i * (cardW + spacing);
        m_cards[i].y = cardY - cardH + 10.0f;
        m_cards[i].w = cardW;
        m_cards[i].h = cardH;
    }
}

int PlayerCards::getCardCost(int cardIndex) const {
    if (cardIndex >= 0 && cardIndex < 3) {
        return m_cards[cardIndex].cost;
    }
    return 0;
}

void PlayerCards::update(int coins, glm::vec2 mousePos, bool leftClicked) {
    m_animTime += 0.016f;

    for (int i = 0; i < 3; ++i) {
        CardData& card = m_cards[i];

        // US-21: Auto-unlock when coins reach cost, auto-relock when below
        card.isUnlocked = (coins >= card.cost);

        // US-19: Locked cards ignore hover and click
        if (!card.isUnlocked) {
            card.isHovered = false;
            // If the selected card became unaffordable, deselect it
            if (m_selectedIndex == i) {
                m_selectedIndex = -1;
            }
            continue;
        }

        // US-20: Unlocked cards respond to hover and click
        card.isHovered = (mousePos.x >= card.x && mousePos.x <= card.x + card.w &&
                          mousePos.y >= card.y && mousePos.y <= card.y + card.h);

        if (card.isHovered && leftClicked) {
            if (m_selectedIndex == i) {
                // Clicking already selected card deselects it
                m_selectedIndex = -1;
                AudioManager::instance().playSound(SoundID::LANE_SELECT);
            } else {
                m_selectedIndex = i;
                AudioManager::instance().playSound(SoundID::PLAYER_PLACE);
            }
        }
    }
}

void PlayerCards::draw() {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();

    for (int i = 0; i < 3; ++i) {
        const CardData& card = m_cards[i];
        bool isSelected = (m_selectedIndex == i);

        // Apply Grayscale uniform if locked (US-19)
        ren.setGrayscale(card.isUnlocked ? 0.0f : 1.0f);

        // Outer border styling
        glm::vec4 borderColor = glm::vec4(0.25f, 0.32f, 0.42f, 1.0f);
        if (isSelected) {
            float pulse = 0.5f + 0.5f * std::sin(m_animTime * 10.0f);
            borderColor = glm::mix(glm::vec4(1.0f, 0.85f, 0.20f, 1.0f), glm::vec4(1.0f, 1.0f, 0.60f, 1.0f), pulse);
        } else if (card.isHovered) {
            borderColor = glm::vec4(0.40f, 0.80f, 1.0f, 1.0f);
        }

        ren.drawRect(card.x - 2, card.y - 2, card.w + 4, card.h + 4, borderColor);

        // Card background
        glm::vec4 bgCol = card.isUnlocked 
            ? (isSelected ? glm::vec4(0.20f, 0.26f, 0.40f, 1.0f) : glm::vec4(0.12f, 0.16f, 0.24f, 1.0f))
            : glm::vec4(0.08f, 0.09f, 0.12f, 1.0f);
        ren.drawRect(card.x, card.y, card.w, card.h, bgCol);

        // Inner frame
        ren.drawRectOutline(card.x + 2, card.y + 2, card.w - 4, card.h - 4, 1.0f,
                            card.isUnlocked ? glm::vec4(0.30f, 0.40f, 0.55f, 0.7f) : glm::vec4(0.18f, 0.18f, 0.20f, 0.5f));

        // Character mini portrait on left of card
        float spriteW = 38.0f, spriteH = 48.0f;
        ren.drawSprite(card.sprite, card.x + 6.0f, card.y + 5.0f, spriteW, spriteH);

        // Reset grayscale so text renders clearly
        ren.setGrayscale(0.0f);

        // Name and Stats
        glm::vec4 nameCol = card.isUnlocked ? glm::vec4(1.0f, 0.95f, 0.70f, 1.0f) : glm::vec4(0.55f, 0.55f, 0.55f, 1.0f);
        tr.drawTextWithShadow(card.name, card.x + 50.0f, card.y + card.h - 18.0f, 1.6f, nameCol);

        glm::vec4 descCol = card.isUnlocked ? glm::vec4(0.75f, 0.85f, 0.95f, 1.0f) : glm::vec4(0.45f, 0.45f, 0.45f, 1.0f);
        tr.drawText(card.desc, card.x + 50.0f, card.y + card.h - 32.0f, 1.2f, descCol);

        // Cost badge with golden shard icon
        if (card.isUnlocked) {
            ren.drawSprite(card.cost > 0 ? "shard" : "shard", card.x + 48.0f, card.y + 6.0f, 16.0f, 16.0f);
            std::string costStr = std::to_string(card.cost);
            tr.drawTextWithShadow(costStr, card.x + 68.0f, card.y + 8.0f, 1.5f, glm::vec4(1.0f, 0.85f, 0.25f, 1.0f));
            if (isSelected) {
                tr.drawTextWithShadow("READY", card.x + card.w - 45.0f, card.y + 8.0f, 1.3f, glm::vec4(0.4f, 1.0f, 0.4f, 1.0f));
            }
        } else {
            tr.drawText("LOCKED (" + std::to_string(card.cost) + ")", card.x + 50.0f, card.y + 8.0f, 1.2f, glm::vec4(0.75f, 0.35f, 0.35f, 1.0f));
        }
    }
    ren.setGrayscale(0.0f);
}
