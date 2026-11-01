#pragma once
// ====================================================================
// FILE:     src/core/Renderer.h
// Common 2D drawing helpers: orthographic projection, quads,
// sprites, transform stack, damage tint, and grayscale.
// ====================================================================

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

class Renderer {
public:
    static Renderer& instance();

    bool init();
    void cleanup();

    void beginFrame();

    // Damage tint (US-43: redder as health drops)
    void setTint(float amount);
    // Grayscale (US-19: locked cards in grayscale)
    void setGrayscale(float amount);

    // Drawing basic geometric shapes
    void drawRect(float x, float y, float w, float h, const glm::vec4& col);
    void drawRectOutline(float x, float y, float w, float h, float thickness, const glm::vec4& col);
    void drawCircle(float cx, float cy, float r, const glm::vec4& col);
    void drawTri(glm::vec2 a, glm::vec2 b, glm::vec2 c, const glm::vec4& col);

    // Drawing textured sprites (Game Boy / Pokemon pixel art!)
    void drawSprite(const std::string& name, float x, float y, float w, float h, bool flipX = false, float rotRad = 0.0f);
    void drawSpriteCentered(const std::string& name, float cx, float cy, float w, float h, bool flipX = false, float rotRad = 0.0f);

    // Transform stack for hierarchical models
    void pushTransform(const glm::mat4& m);
    void popTransform();
    glm::mat4 currentTransform() const;

    const Shader& getShader() const { return m_shader; }
    const glm::mat4& getProjection() const { return m_proj; }

private:
    Renderer() = default;
    ~Renderer() = default;

    Shader m_shader;
    glm::mat4 m_proj{1.0f};

    GLuint m_quadVAO = 0;
    GLuint m_quadVBO = 0;

    GLuint m_circleVAO = 0;
    GLuint m_circleVBO = 0;
    int m_circleVertexCount = 0;

    GLuint m_triVAO = 0;
    GLuint m_triVBO = 0;

    float m_tint = 0.0f;
    float m_grayscale = 0.0f;

    std::vector<glm::mat4> m_matrixStack;
};
