#include "PropModels.h"
#include "../core/Renderer.h"

void PropModels::drawMeleeWeapon(float x, float y, float rot) {
    Renderer::instance().drawSpriteCentered("sword", x, y, 36.0f, 36.0f, false, rot);
}

void PropModels::drawRangedWeapon(float x, float y, float rot) {
    Renderer::instance().drawSpriteCentered("shuriken", x, y, 26.0f, 26.0f, false, rot);
}

void PropModels::drawProjectile(const std::string& type, float x, float y, float rot) {
    Renderer::instance().drawSpriteCentered(type, x, y, 28.0f, 28.0f, false, rot);
}

void PropModels::drawCollectible(float x, float y, float rot) {
    Renderer::instance().drawSpriteCentered("shard", x, y, 32.0f, 32.0f, false, rot);
}
