#include "M3Draw.h"

#include "../core/Renderer.h"
#include "../ui/TextRenderer.h"

#include <chrono>
#include <algorithm>

namespace
{
const auto startTime = std::chrono::steady_clock::now();

std::string spriteName(const char* path)
{
    std::string name(path);
    const std::string::size_type separator = name.find_last_of("/\\");
    if (separator != std::string::npos)
        name.erase(0, separator + 1);
    const std::string::size_type extension = name.find_last_of('.');
    if (extension != std::string::npos)
        name.erase(extension);
    return name;
}
}

namespace M3Draw
{
void quad(float x, float y, float width, float height,
          float r, float g, float b, float alpha)
{
    Renderer::instance().drawRect(x, y, width, height, glm::vec4(r, g, b, alpha));
}

void circle(float cx, float cy, float radius,
            float r, float g, float b, float alpha, int)
{
    Renderer::instance().drawCircle(cx, cy, radius, glm::vec4(r, g, b, alpha));
}

void triangle(float x1, float y1, float x2, float y2, float x3, float y3,
              float r, float g, float b, float alpha)
{
    Renderer::instance().drawTri(glm::vec2(x1, y1), glm::vec2(x2, y2),
                                 glm::vec2(x3, y3), glm::vec4(r, g, b, alpha));
}

void line(float x1, float y1, float x2, float y2, float thickness,
          float r, float g, float b, float alpha)
{
    const glm::vec2 direction(x2 - x1, y2 - y1);
    const float length = glm::length(direction);
    if (length <= 0.0f)
        return;

    const glm::vec2 perpendicular(-direction.y / length, direction.x / length);
    const float halfThickness = thickness * 0.5f;
    glm::vec2 normal = perpendicular * halfThickness;
    Renderer::instance().drawTri(glm::vec2(x1, y1) - normal,
                                 glm::vec2(x2, y2) - normal,
                                 glm::vec2(x2, y2) + normal,
                                 glm::vec4(r, g, b, alpha));
    Renderer::instance().drawTri(glm::vec2(x1, y1) - normal,
                                 glm::vec2(x2, y2) + normal,
                                 glm::vec2(x1, y1) + normal,
                                 glm::vec4(r, g, b, alpha));
}

void text(float x, float y, const char* value, float scale,
          float r, float g, float b, float alpha)
{
    TextRenderer::instance().drawText(value, x, y, scale, glm::vec4(r, g, b, alpha));
}

void image(const char* path, float x, float y, float width, float height, float alpha)
{
    Renderer::instance().drawSprite(spriteName(path), x, y, width, height);
}

void imageRegion(const char* path, float x, float y, float width, float height,
                 float, float, float, float)
{
    Renderer::instance().drawSprite(spriteName(path), x, y, width, height);
}

float getTime()
{
    return std::chrono::duration<float>(std::chrono::steady_clock::now() - startTime).count();
}
}
