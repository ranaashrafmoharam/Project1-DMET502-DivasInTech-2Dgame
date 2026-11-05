#include "HealthBar.h"
#include "scene/Scene.h"

void HealthBar::draw(float x, float y, float width, int health, int maxHealth, bool enemy)
{
    if (maxHealth <= 0)
        return;
    float ratio = static_cast<float>(health) / maxHealth;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    M3Draw::quad(x - width * 0.5f - 2.0f, y, width + 4.0f, 8.0f, 0.08f, 0.05f, 0.06f, 0.95f);
    M3Draw::quad(x - width * 0.5f, y + 2.0f, width * ratio, 4.0f,
                 enemy ? 0.78f : 0.18f,
                 enemy ? 0.18f : 0.75f,
                 enemy ? 0.18f : 0.25f, 1.0f);
}
