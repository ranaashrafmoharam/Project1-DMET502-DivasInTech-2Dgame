#pragma once
// ====================================================================
// FILE:     src/scene/HomeBase.h
// OWNER:    Member 3
// TASKS:    M3-08
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   The player's safe zone at the left end of the playfield.
//
// RESPONSIBILITIES:
//   - Detailed structure, clearly different from the environment
//   - Highly visible against the background
//   - Its right edge is the losing line used by Member 1
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - draw()
//   - getBounds()
//
// INTERACTS WITH: Scene, CollisionSystem
// ====================================================================

#pragma once

struct HomeBaseBounds
{
    float left;
    float right;
    float bottom;
    float top;
};

class HomeBase
{
public:
    void draw();
    HomeBaseBounds getBounds() const;
};
