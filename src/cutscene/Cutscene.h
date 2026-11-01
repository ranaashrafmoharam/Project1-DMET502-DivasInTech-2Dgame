#pragma once
// ====================================================================
// FILE:     src/cutscene/Cutscene.h
// OWNER:    Member 4
// TASKS:    M4-09
// Animated intro cutscene: timeline-driven, Ninjago confrontation,
// dialogue subtitles, visibly animated heroes and villains, skip key.
// ====================================================================

#include <glm/glm.hpp>
#include <string>

class Cutscene {
public:
    static Cutscene& instance();

    void reset();
    void update(float dt);
    void draw();

    bool isFinished() const { return m_finished; }
    void skip();

private:
    Cutscene() = default;

    float m_timeline = 0.0f;
    bool m_finished = false;

    // Animated character positions
    float m_wuX = 220.0f;
    float m_kaiX = -120.0f;
    float m_garmadonX = 1380.0f;
    float m_venomariX = 1420.0f;

    // Combat clash animation
    float m_acidX = 0.0f;
    float m_acidY = 0.0f;
    bool m_acidActive = false;
    float m_slashRot = 0.0f;
    bool m_slashActive = false;
};
