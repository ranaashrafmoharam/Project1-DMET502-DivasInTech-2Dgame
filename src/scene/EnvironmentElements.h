#pragma once
// ====================================================================
// FILE:     src/scene/EnvironmentElements.h
// 5 themed environmental elements: Cherry tree, stone lantern,
// training dummy, prayer gong, dragon banner.
// ====================================================================

class EnvironmentElements {
public:
    static EnvironmentElements& instance();

    void update(float dt);
    void draw();

private:
    EnvironmentElements() = default;

    float m_animTime = 0.0f;
};
