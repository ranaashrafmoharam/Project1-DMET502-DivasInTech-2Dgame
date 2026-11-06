#pragma once

#include <string>

namespace M3Draw
{
void quad(float x, float y, float width, float height,
          float r, float g, float b, float alpha = 1.0f);
void circle(float cx, float cy, float radius,
            float r, float g, float b, float alpha = 1.0f, int segments = 16);
void triangle(float x1, float y1, float x2, float y2, float x3, float y3,
              float r, float g, float b, float alpha = 1.0f);
void line(float x1, float y1, float x2, float y2, float thickness,
          float r, float g, float b, float alpha = 1.0f);
void text(float x, float y, const char* value, float scale,
          float r, float g, float b, float alpha = 1.0f);
void image(const char* path, float x, float y, float width, float height, float alpha = 1.0f);
void imageRegion(const char* path, float x, float y, float width, float height,
                 float sourceX0, float sourceY0, float sourceX1, float sourceY1);
float getTime();
}
