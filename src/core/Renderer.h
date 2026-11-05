#pragma once
// ====================================================================
// FILE:     src/core/Renderer.h
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Common drawing helpers used by everybody: projection setup and
//   basic draw calls with colour, tint and grayscale options.
//
// RESPONSIBILITIES:
//   - Set up the 2D orthographic projection
//   - Draw coloured/textured quads and shapes
//   - Draw with a red damage tint or grayscale (locked cards)
//   - Provide a transform stack used by hierarchical models
// ====================================================================

#if __has_include(<glad/glad.h>)
#include <glad/glad.h>
#elif __has_include(<glad/gl.h>)
#include <glad/gl.h>
#endif

#include <glm/glm.hpp>
#include <vector>
#include <stack>
#include "Shader.h"
#include "Math.h"
#include "Constants.h"

class Renderer {
public:
    Renderer();
    virtual ~Renderer();
    static Renderer& getInstance();

    bool init();
    void shutdown();

    // Projection & Views
    void setProjection(float width, float height);
    const glm::mat4& getProjection() const { return m_projection; }

    // Transform stack (used by hierarchical models, M2-01)
    void beginFrame();
    void setDefaultOrtho() { setProjection(static_cast<float>(Constants::SCREEN_WIDTH), static_cast<float>(Constants::SCREEN_HEIGHT)); }
    void pushTransform();
    void pushTransform(const Transform2D& transform);
    void popTransform();
    const Transform2D& top() const { return m_characterTransformStack.back(); }
    void applyTransform(const glm::mat4& transform);
    void resetTransform();
    const glm::mat4& getCurrentTransform() const;

    // Shader states
    void setTint(const glm::vec4& tint);
    void setTint(const Vec4& tint);
    void setTintAmount(float amount);
    const Vec4& getTint() const { return m_characterTint; }
    float getTintAmount() const { return m_characterTintAmount; }
    void setGrayscale(bool enabled);
    bool isGrayscale() const { return m_currentGrayscale; }
    void resetShaderState();

    // Drawing helpers
    // Top-left x, y
    void drawQuad(float x, float y, float width, float height, const glm::vec4& color, unsigned int textureID = 0);
    // Centered at x, y
    void drawQuadCentered(float x, float y, float width, float height, const glm::vec4& color, unsigned int textureID = 0);
    virtual void drawQuad(const Vec2& center, const Vec2& size, const Vec4& color, float rotation = 0.0f);
    virtual void drawCircle(const Vec2& center, float radius, const Vec4& color, int segments = 20);
    virtual void drawTriangle(const Vec2& a, const Vec2& b, const Vec2& c, const Vec4& color);
    virtual void drawTrapezoid(const Vec2& center, const Vec2& size, float topWidthScale,
                               const Vec4& color, float rotation = 0.0f);
    // Draw hollow/bordered rectangle
    void drawBorder(float x, float y, float width, float height, float thickness, const glm::vec4& color);

    // Bitmap text rendering helpers
    void drawText(const std::string& text, float x, float y, float scale = 2.0f, const glm::vec4& color = glm::vec4(1.0f));
    void drawTextCentered(const std::string& text, float centerX, float centerY, float scale = 2.0f, const glm::vec4& color = glm::vec4(1.0f));
    float getTextWidth(const std::string& text, float scale = 2.0f);

    Shader& getShader() { return m_shader; }

private:
    Shader m_shader;
    glm::mat4 m_projection;
    std::stack<glm::mat4> m_transformStack;

    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    unsigned int m_quadEBO;

    glm::vec4 m_currentTint;
    glm::vec4 m_tintBase;
    float m_tintAmount;
    Vec4 m_characterTint{ 1.0f, 1.0f, 1.0f, 1.0f };
    float m_characterTintAmount = 0.0f;
    std::vector<Transform2D> m_characterTransformStack{ Transform2D{} };
    bool m_currentGrayscale;

    void setupQuadBuffers();
};

inline Renderer*& rendererSlot() {
    static Renderer* renderer = nullptr;
    return renderer;
}
inline Renderer& getRenderer() {
    return rendererSlot() ? *rendererSlot() : Renderer::getInstance();
}
inline void setRenderer(Renderer* renderer) { rendererSlot() = renderer; }
