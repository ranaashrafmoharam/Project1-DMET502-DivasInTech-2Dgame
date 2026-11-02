#pragma once
// ====================================================================
// FILE:     src/scene/Scene.h
// OWNER:    Member 3
// TASKS:    M3-02
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Draws the whole game world behind the units.
//
// RESPONSIBILITIES:
//   - Background and lane ground strips with visible lane separation
//   - Call the environment elements and the home base
//   - Keep the scene readable against characters
//   - Provide high-performance Game Boy / Pokemon style pixel art drawing
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - init()
//   - draw()
//
// INTERACTS WITH: EnvironmentElements, HomeBase, Game, Cutscene
// ====================================================================

#include "EnvironmentElements.h"
#include "HomeBase.h"
#include <map>
#include <vector>

class Scene
{
public:
    void init();
    void draw(bool includeLanterns = true, bool includeLeftLantern = true);
};

// Member 3 drawing helpers shared by the scene, environment and props.
namespace M3Draw
{
void init(float screenWidth = 1280.0f, float screenHeight = 720.0f);
void flush();
void quad(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f);
void image(const char* path, float x, float y, float w, float h, float alpha = 1.0f);
void imageRegion(const char* path, float x, float y, float w, float h,
                 float u0, float v0, float u1, float v1, float alpha = 1.0f);
void circle(float cx, float cy, float radius, float r, float g, float b, float a = 1.0f, int segments = 24);
void triangle(float x1, float y1, float x2, float y2, float x3, float y3,
              float r, float g, float b, float a = 1.0f);
void line(float x1, float y1, float x2, float y2, float thickness,
          float r, float g, float b, float a = 1.0f);

// Pixel art text & font helpers
void text(float x, float y, const char* str, float scale = 2.0f,
          float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
float textWidth(const char* str, float scale = 2.0f);

// Time query for ambient environment animations
float getTime();
void advanceTime(float dt);
}
