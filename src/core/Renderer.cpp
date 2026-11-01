#include "Renderer.h"
#include "Constants.h"
#include "Texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

Renderer& Renderer::instance() {
    static Renderer ren;
    return ren;
}

bool Renderer::init() {
    m_proj = glm::ortho(0.0f, Constants::WINDOW_WIDTH, 0.0f, Constants::WINDOW_HEIGHT, -1.0f, 1.0f);

    // Load basic shader
    if (!m_shader.loadFromFiles("assets/shaders/basic.vert", "assets/shaders/basic.frag")) {
        std::cerr << "Renderer: failed to load basic shaders from files, compiling fallback..." << std::endl;
        const char* fallbackVS = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
uniform mat4 uProj;
uniform mat4 uModel;
out vec2 TexCoord;
void main() { gl_Position = uProj * uModel * vec4(aPos, 0.0, 1.0); TexCoord = aTexCoord; })";

        const char* fallbackFS = R"(#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform vec4 uColor;
uniform float uRed;
uniform float uGray;
uniform int uUseTexture;
uniform sampler2D uTexture;
void main() {
    vec4 c = uColor;
    if (uUseTexture == 1) { c *= texture(uTexture, TexCoord); }
    if (c.a < 0.05) discard;
    vec3 tinted = mix(c.rgb, vec3(1.0, 0.12, 0.12), uRed * 0.85);
    float gray = dot(tinted, vec3(0.299, 0.587, 0.114));
    FragColor = vec4(mix(tinted, vec3(gray), uGray), c.a);
})";
        m_shader.loadFromStrings(fallbackVS, fallbackFS);
    }

    // 1. Unit quad: (x, y, u, v)
    // In OpenGL texture coords: (0, 1) top-left, (1, 0) bottom-right
    float quadVertices[] = {
        // Pos      // UV
        0.0f, 1.0f, 0.0f, 0.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, // top-right
        1.0f, 0.0f, 1.0f, 1.0f, // bottom-right

        0.0f, 1.0f, 0.0f, 0.0f, // top-left
        1.0f, 0.0f, 1.0f, 1.0f, // bottom-right
        0.0f, 0.0f, 0.0f, 1.0f  // bottom-left
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 2. Unit circle fan
    std::vector<float> circleData;
    circleData.push_back(0.0f); circleData.push_back(0.0f);
    circleData.push_back(0.5f); circleData.push_back(0.5f);
    const int segments = 40;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159265f * (float)i / (float)segments;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        circleData.push_back(cosA);
        circleData.push_back(sinA);
        circleData.push_back(0.5f + 0.5f * cosA);
        circleData.push_back(0.5f + 0.5f * sinA);
    }
    m_circleVertexCount = segments + 2;

    glGenVertexArrays(1, &m_circleVAO);
    glGenBuffers(1, &m_circleVBO);
    glBindVertexArray(m_circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circleData.size() * sizeof(float), circleData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 3. Unit right triangle
    float triVertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };
    glGenVertexArrays(1, &m_triVAO);
    glGenBuffers(1, &m_triVBO);
    glBindVertexArray(m_triVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    m_matrixStack.push_back(glm::mat4(1.0f));
    return true;
}

void Renderer::cleanup() {
    if (m_quadVAO != 0) glDeleteVertexArrays(1, &m_quadVAO);
    if (m_quadVBO != 0) glDeleteBuffers(1, &m_quadVBO);
    if (m_circleVAO != 0) glDeleteVertexArrays(1, &m_circleVAO);
    if (m_circleVBO != 0) glDeleteBuffers(1, &m_circleVBO);
    if (m_triVAO != 0) glDeleteVertexArrays(1, &m_triVAO);
    if (m_triVBO != 0) glDeleteBuffers(1, &m_triVBO);
}

void Renderer::beginFrame() {
    m_shader.use();
    m_shader.setMat4("uProj", m_proj);
    m_shader.setFloat("uRed", m_tint);
    m_shader.setFloat("uGray", m_grayscale);
    m_shader.setInt("uTexture", 0);
}

void Renderer::setTint(float amount) {
    m_tint = glm::clamp(amount, 0.0f, 1.0f);
    m_shader.setFloat("uRed", m_tint);
}

void Renderer::setGrayscale(float amount) {
    m_grayscale = glm::clamp(amount, 0.0f, 1.0f);
    m_shader.setFloat("uGray", m_grayscale);
}

void Renderer::drawRect(float x, float y, float w, float h, const glm::vec4& col) {
    m_shader.use();
    glm::mat4 model = currentTransform();
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    m_shader.setMat4("uModel", model);
    m_shader.setVec4("uColor", col);
    m_shader.setInt("uUseTexture", 0);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::drawRectOutline(float x, float y, float w, float h, float thickness, const glm::vec4& col) {
    // Top
    drawRect(x, y + h - thickness, w, thickness, col);
    // Bottom
    drawRect(x, y, w, thickness, col);
    // Left
    drawRect(x, y, thickness, h, col);
    // Right
    drawRect(x + w - thickness, y, thickness, h, col);
}

void Renderer::drawCircle(float cx, float cy, float r, const glm::vec4& col) {
    m_shader.use();
    glm::mat4 model = currentTransform();
    model = glm::translate(model, glm::vec3(cx, cy, 0.0f));
    model = glm::scale(model, glm::vec3(r, r, 1.0f));

    m_shader.setMat4("uModel", model);
    m_shader.setVec4("uColor", col);
    m_shader.setInt("uUseTexture", 0);

    glBindVertexArray(m_circleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_circleVertexCount);
    glBindVertexArray(0);
}

void Renderer::drawTri(glm::vec2 a, glm::vec2 b, glm::vec2 c, const glm::vec4& col) {
    m_shader.use();
    glm::mat4 M(1.0f);
    M[0] = glm::vec4(b.x - a.x, b.y - a.y, 0.0f, 0.0f);
    M[1] = glm::vec4(c.x - a.x, c.y - a.y, 0.0f, 0.0f);
    M[3] = glm::vec4(a.x, a.y, 0.0f, 1.0f);

    glm::mat4 model = currentTransform() * M;
    m_shader.setMat4("uModel", model);
    m_shader.setVec4("uColor", col);
    m_shader.setInt("uUseTexture", 0);

    glBindVertexArray(m_triVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Renderer::drawSprite(const std::string& name, float x, float y, float w, float h, bool flipX, float rotRad) {
    TextureInfo tex = TextureManager::instance().get(name);
    if (tex.id == 0) {
        // Fallback placeholder rect if texture missing
        drawRect(x, y, w, h, glm::vec4(0.8f, 0.2f, 0.8f, 1.0f));
        return;
    }

    m_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glm::mat4 model = currentTransform();
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    if (rotRad != 0.0f) {
        model = glm::translate(model, glm::vec3(w * 0.5f, h * 0.5f, 0.0f));
        model = glm::rotate(model, rotRad, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-w * 0.5f, -h * 0.5f, 0.0f));
    }
    if (flipX) {
        model = glm::translate(model, glm::vec3(w, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(-w, h, 1.0f));
    } else {
        model = glm::scale(model, glm::vec3(w, h, 1.0f));
    }

    m_shader.setMat4("uModel", model);
    m_shader.setVec4("uColor", glm::vec4(1.0f));
    m_shader.setInt("uUseTexture", 1);
    m_shader.setInt("uTexture", 0);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::drawSpriteCentered(const std::string& name, float cx, float cy, float w, float h, bool flipX, float rotRad) {
    drawSprite(name, cx - w * 0.5f, cy - h * 0.5f, w, h, flipX, rotRad);
}

void Renderer::pushTransform(const glm::mat4& m) {
    m_matrixStack.push_back(currentTransform() * m);
}

void Renderer::popTransform() {
    if (m_matrixStack.size() > 1) {
        m_matrixStack.pop_back();
    }
}

glm::mat4 Renderer::currentTransform() const {
    return m_matrixStack.empty() ? glm::mat4(1.0f) : m_matrixStack.back();
}
