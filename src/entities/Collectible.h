#pragma once
// ====================================================================
// FILE:     src/entities/Collectible.h
// OWNER:    Member 4
// TASKS:    M4-08
// Golden Weapon Shard dropped by defeated enemies. Spins and bobs.
// ====================================================================

#include "Entity.h"

class Collectible : public Entity {
public:
    Collectible(int laneIdx, float startX, float startY);

    void update(float dt) override;
    void draw() override;

    int getValue() const { return m_value; }
    bool isCollected() const { return m_collected; }
    void collect();

    bool isDone() const { return m_done; }

private:
    int m_value = 10;
    float m_animTime = 0.0f;
    bool m_collected = false;
    bool m_done = false;
    float m_floatTimer = 0.0f;
};
