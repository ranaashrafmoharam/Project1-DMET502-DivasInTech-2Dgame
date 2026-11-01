#pragma once
// ====================================================================
// FILE:     src/scene/PropModels.h
// Visual helpers for props: weapons, projectiles, collectibles.
// ====================================================================

#include <glm/glm.hpp>
#include <string>

class PropModels {
public:
    static void drawMeleeWeapon(float x, float y, float rot = 0.0f);
    static void drawRangedWeapon(float x, float y, float rot = 0.0f);
    static void drawProjectile(const std::string& type, float x, float y, float rot = 0.0f);
    static void drawCollectible(float x, float y, float rot = 0.0f);
};
