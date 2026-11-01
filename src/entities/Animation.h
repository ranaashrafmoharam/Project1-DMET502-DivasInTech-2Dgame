#pragma once
// ====================================================================
// FILE:     src/entities/Animation.h
// Time-driven poses and walk cycles.
// ====================================================================

class Animation {
public:
    static float getWalkBob(float time, float speed = 8.0f, float amplitude = 3.0f);
    static float getLimbSwing(float time, float speed = 8.0f, float maxAngle = 0.4f);
};
