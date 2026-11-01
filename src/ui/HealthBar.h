#pragma once
// ====================================================================
// FILE:     src/ui/HealthBar.h
// OWNER:    Member 4
// TASKS:    M4-07
// Pokemon style health bars and red damage tint feedback.
// ====================================================================

#include <glm/glm.hpp>

class HealthBar {
public:
    static HealthBar& instance();

    // Renders Pokemon HP bar above or below a unit
    void draw(float entityX, float entityY, float entityW, float entityH, 
              int currentHealth, int maxHealth, bool isEnemy = false);

    // Calculates damage red tint intensity (0.0 .. 1.0) for basic shader (US-43)
    float getTintAmount(int currentHealth, int maxHealth) const;

private:
    HealthBar() = default;
};
