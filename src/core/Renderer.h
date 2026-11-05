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

class Renderer {
public:
    static Renderer& getInstance();

    bool init();
    void shutdown();

    // Projection & Views
    void setProjection(float width, float height);
    const glm::mat4& getProjection() const { return m_projection; }

    // Transform stack (used by hierarchical models, M2-01)
    void pushTransform();
    void popTransform();
    void applyTransform(const glm::mat4& transform);
    void resetTransform();
    const glm::mat4& getCurrentTransform() const;

    // Shader states
    void setTint(const glm::vec4& tint);
    void setGrayscale(bool enabled);
    void resetShaderState();

    // Drawing helpers
    // Top-left x, y
    void drawQuad(float x, float y, float width, float height, const glm::vec4& color, unsigned int textureID = 0);
    // Centered at x, y
    void drawQuadCentered(float x, float y, float width, float height, const glm::vec4& color, unsigned int textureID = 0);
    // Draw hollow/bordered rectangle
    void drawBorder(float x, float y, float width, float height, float thickness, const glm::vec4& color);

    // Bitmap text rendering helpers
    void drawText(const std::string& text, float x, float y, float scale = 2.0f, const glm::vec4& color = glm::vec4(1.0f));
    void drawTextCentered(const std::string& text, float centerX, float centerY, float scale = 2.0f, const glm::vec4& color = glm::vec4(1.0f));
    float getTextWidth(const std::string& text, float scale = 2.0f);

    Shader& getShader() { return m_shader; }

private:
    Renderer();
    ~Renderer();

    Shader m_shader;
    glm::mat4 m_projection;
    std::stack<glm::mat4> m_transformStack;

    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    unsigned int m_quadEBO;

    glm::vec4 m_currentTint;
    bool m_currentGrayscale;

    void setupQuadBuffers();
};
