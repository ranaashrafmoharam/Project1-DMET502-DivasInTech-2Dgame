#pragma once
// ====================================================================
// FILE:     src/scene/EnvironmentElements.h
// OWNER:    Member 3
// TASKS:    M3-03 to M3-07
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   The five or more themed environment objects.
//
// RESPONSIBILITIES:
//   - Five distinct elements with their own detail
//   - Placed naturally without blocking gameplay
//   - Optional ambient animation
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - drawElement1() ... drawElement5()
//
// INTERACTS WITH: Scene
// ====================================================================

#pragma once

class EnvironmentElements
{
public:
    void drawElement1(); // Torii gate
    void drawElement2(); // Curved monastery roof
    void drawElement3(); // Clan banners
    void drawElement4(bool includeLeftLantern = true); // Stone lanterns
    void drawElement5(); // Training posts and weapon rack
    void drawElement6(); // Cherry tree
};
