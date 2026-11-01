#pragma once
// ====================================================================
// FILE:     src/scene/Scene.h
// Draws the monastery background, lanes, home base, and environment.
// ====================================================================

class Scene {
public:
    static Scene& instance();

    void init();
    void update(float dt);
    void draw();

private:
    Scene() = default;
};
