#include "Collectible.h"
#include "scene/Scene.h"
#include <cmath>

Collectible::Collectible(float xPosition, float yPosition, int laneIndex, int shardValue)
    : x(xPosition), baseY(yPosition), lane(laneIndex), value(shardValue) {}

void Collectible::update(float dt)
{
    if (!collected)
        animationTime += dt;
}

void Collectible::draw(float time)
{
    if (collected)
        return;

    const float bobY = baseY - 12.0f + std::sin(animationTime * 4.0f) * 5.0f;
    const float pulse = 14.0f + 4.0f * (0.5f + 0.5f * std::sin(animationTime * 6.0f));
    M3Draw::circle(x, bobY, pulse, 1.0f, 0.85f, 0.2f, 0.25f, 16);
    M3Draw::triangle(x - 10.0f, bobY - 12.0f, x + 10.0f, bobY - 12.0f,
                     x, bobY + 14.0f, 1.0f, 0.80f, 0.20f);
    const float width = 8.0f + 6.0f * std::abs(std::sin(time + animationTime));
    M3Draw::quad(x - width * 0.5f, bobY - 14.0f, width, 28.0f, 1.0f, 0.78f, 0.18f);
}

bool Collectible::overlapsPlayer(float playerX) const
{
    return !collected && std::abs(playerX - x) < 30.0f;
}

int Collectible::collect()
{
    if (collected)
        return 0;
    collected = true;
    return value;
}
