#include "HealthBar.h"
#include "../core/Renderer.h"
#include "TextRenderer.h"
#include <string>
#include <algorithm>

HealthBar& HealthBar::instance() {
    static HealthBar hb;
    return hb;
}

float HealthBar::getTintAmount(int currentHealth, int maxHealth) const {
    if (maxHealth <= 0) return 0.0f;
    float ratio = (float)currentHealth / (float)maxHealth;
    // As health drops, red tint increases (US-43)
    return glm::clamp((1.0f - ratio) * 0.90f, 0.0f, 0.90f);
}

void HealthBar::draw(float entityX, float entityY, float entityW, float entityH,
                     int currentHealth, int maxHealth, bool isEnemy) {
    if (maxHealth <= 0) return;
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();

    float barW = 56.0f;
    float barH = 7.0f;
    float barX = entityX + (entityW - barW) * 0.5f;
    float barY = entityY + entityH + 8.0f; // placed directly above unit

    // Outer black border frame (classic Pokemon HP bar look)
    ren.drawRect(barX - 2, barY - 2, barW + 4, barH + 4, glm::vec4(0.04f, 0.05f, 0.08f, 1.0f));

    // Dark background groove
    ren.drawRect(barX, barY, barW, barH, glm::vec4(0.20f, 0.22f, 0.25f, 1.0f));

    // Health ratio
    float ratio = glm::clamp((float)currentHealth / (float)maxHealth, 0.0f, 1.0f);

    // Pokemon HP color standard:
    // Green > 50%, Yellow 20%-50%, Red < 20%
    glm::vec4 hpColor;
    if (ratio > 0.50f) {
        hpColor = glm::vec4(0.25f, 0.85f, 0.35f, 1.0f); // Green
    } else if (ratio > 0.20f) {
        hpColor = glm::vec4(0.95f, 0.80f, 0.20f, 1.0f); // Yellow
    } else {
        hpColor = glm::vec4(0.95f, 0.22f, 0.22f, 1.0f); // Red
    }

    // Fill
    if (ratio > 0.0f) {
        ren.drawRect(barX, barY, barW * ratio, barH, hpColor);
    }

    // Tiny "HP" label tag on left of the bar
    tr.drawText("HP", barX - 16.0f, barY, 1.0f, glm::vec4(1.0f, 0.85f, 0.25f, 1.0f));

    // Numerical health (e.g. "5/5")
    std::string numStr = std::to_string(std::max(0, currentHealth)) + "/" + std::to_string(maxHealth);
    glm::vec4 numCol = isEnemy ? glm::vec4(1.0f, 0.70f, 0.70f, 1.0f) : glm::vec4(0.85f, 0.95f, 1.0f, 1.0f);
    tr.drawText(numStr, barX + barW + 4.0f, barY - 1.0f, 1.0f, numCol);
}
