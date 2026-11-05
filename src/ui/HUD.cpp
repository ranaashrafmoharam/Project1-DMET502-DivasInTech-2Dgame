#include "HUD.h"
#include "scene/Scene.h"
#include "scene/PropModels.h"
#include <cstdio>

void HUD::draw(const GameplayHudState& state) const
{
    const float x = 20.0f;
    M3Draw::quad(x, 8.0f, 230.0f, 110.0f, 0.08f, 0.07f, 0.14f, 0.95f);
    M3Draw::quad(x + 2.0f, 10.0f, 226.0f, 106.0f, 0.88f, 0.76f, 0.32f, 0.95f);
    M3Draw::quad(x + 5.0f, 13.0f, 220.0f, 100.0f, 0.12f, 0.11f, 0.18f, 0.95f);
    M3Draw::text(x + 72.0f, 30.0f, "GOLDEN SHARDS", 1.55f, 1.0f, 0.90f, 0.40f);

    PropModels props;
    props.drawCollectible(x + 38.0f, 44.0f, M3Draw::getTime());

    char text[32];
    std::snprintf(text, sizeof(text), "%d", state.coins);
    M3Draw::text(x + 72.0f, 54.0f, text, 2.5f, 1.0f, 0.98f, 0.65f);

    M3Draw::quad(x, 122.0f, 230.0f, 54.0f, 0.08f, 0.07f, 0.14f, 0.95f);
    M3Draw::quad(x + 2.0f, 124.0f, 226.0f, 50.0f, 0.20f, 0.18f, 0.27f, 0.95f);
    std::snprintf(text, sizeof(text), "WAVE %d / %d", state.currentWave, state.totalWaves);
    M3Draw::text(x + 12.0f, 132.0f, text, 1.35f, 1.0f, 0.88f, 0.32f);
    std::snprintf(text, sizeof(text), "ENEMIES LEFT: %d", state.enemiesRemaining);
    M3Draw::text(x + 12.0f, 151.0f, text, 1.25f, 0.90f, 0.90f, 1.0f);

    M3Draw::quad(x, 180.0f, 230.0f, 70.0f, 0.08f, 0.07f, 0.14f, 0.95f);
    M3Draw::quad(x + 2.0f, 182.0f, 230.0f - 4.0f, 66.0f, 0.20f, 0.18f, 0.27f, 0.95f);
    M3Draw::text(x + 12.0f, 190.0f, "MONASTERY GATE", 1.35f, 1.0f, 0.88f, 0.32f);
    M3Draw::quad(x + 12.0f, 211.0f, 196.0f, 12.0f, 0.08f, 0.05f, 0.06f, 1.0f);
    float ratio = state.gateMaxHealth > 0
        ? static_cast<float>(state.gateHealth) / state.gateMaxHealth : 0.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    M3Draw::quad(x + 14.0f, 213.0f, 192.0f * ratio, 8.0f,
                 ratio > 0.5f ? 0.30f : 0.85f, ratio > 0.5f ? 0.78f : 0.20f, 0.30f, 1.0f);
    std::snprintf(text, sizeof(text), "BASE INTEGRITY: %d%%", static_cast<int>(ratio * 100.0f));
    M3Draw::text(x + 12.0f, 232.0f, text, 1.15f, 0.82f, 1.0f, 0.82f);

    if (!state.notification.empty() && state.notificationAlpha > 0.0f)
    {
        M3Draw::quad(x, 258.0f, 230.0f, 34.0f, 0.08f, 0.07f, 0.14f, state.notificationAlpha * 0.95f);
        M3Draw::text(x + 12.0f, 268.0f, state.notification.c_str(), 1.2f,
                     1.0f, 0.90f, 0.30f, state.notificationAlpha);
    }
}
