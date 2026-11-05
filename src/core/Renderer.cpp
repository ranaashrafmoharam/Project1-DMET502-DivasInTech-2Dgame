// ====================================================================
// FILE:     src/core/Renderer.cpp
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   IMPLEMENTED
// ====================================================================

#include "Renderer.h"
#include "Constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer& Renderer::getInstance() {
    static Renderer instance;
    return instance;
}

Renderer::Renderer()
    : m_projection(1.0f)
    , m_quadVAO(0)
    , m_quadVBO(0)
    , m_quadEBO(0)
    , m_currentTint(1.0f, 1.0f, 1.0f, 1.0f)
    , m_currentGrayscale(false)
{
    m_transformStack.push(glm::mat4(1.0f));
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    // Load default shader
    if (!m_shader.load("assets/shaders/basic.vert", "assets/shaders/basic.frag")) {
        std::cerr << "[Renderer] Warning: Could not load shaders from assets/shaders, attempting fallback paths..." << std::endl;
        if (!m_shader.load("../assets/shaders/basic.vert", "../assets/shaders/basic.frag")) {
            std::cerr << "[Renderer] Error: Failed to load basic shaders!" << std::endl;
            return false;
        }
    }

    setProjection((float)Constants::SCREEN_WIDTH, (float)Constants::SCREEN_HEIGHT);
    setupQuadBuffers();

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Renderer::shutdown() {
    if (m_quadVAO != 0) {
        glDeleteVertexArrays(1, &m_quadVAO);
        m_quadVAO = 0;
    }
    if (m_quadVBO != 0) {
        glDeleteBuffers(1, &m_quadVBO);
        m_quadVBO = 0;
    }
    if (m_quadEBO != 0) {
        glDeleteBuffers(1, &m_quadEBO);
        m_quadEBO = 0;
    }
}

void Renderer::setupQuadBuffers() {
    // Unit quad with (0,0) at top-left and (1,1) at bottom-right
    float vertices[] = {
        // x,     y,     u,    v
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glGenBuffers(1, &m_quadEBO);

    glBindVertexArray(m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position (layout = 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TexCoord (layout = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::setProjection(float width, float height) {
    // Standard 2D orthographic: (0,0) top-left, (width, height) bottom-right
    m_projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

void Renderer::pushTransform() {
    m_transformStack.push(m_transformStack.top());
}

void Renderer::popTransform() {
    if (m_transformStack.size() > 1) {
        m_transformStack.pop();
    }
}

void Renderer::applyTransform(const glm::mat4& transform) {
    m_transformStack.top() = m_transformStack.top() * transform;
}

void Renderer::resetTransform() {
    while (m_transformStack.size() > 1) {
        m_transformStack.pop();
    }
    m_transformStack.top() = glm::mat4(1.0f);
}

const glm::mat4& Renderer::getCurrentTransform() const {
    return m_transformStack.top();
}

void Renderer::setTint(const glm::vec4& tint) {
    m_currentTint = tint;
}

void Renderer::setGrayscale(bool enabled) {
    m_currentGrayscale = enabled;
}

void Renderer::resetShaderState() {
    m_currentTint = glm::vec4(1.0f);
    m_currentGrayscale = false;
}

void Renderer::drawQuad(float x, float y, float width, float height, const glm::vec4& color, unsigned int textureID) {
    glm::mat4 model = m_transformStack.top();
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    m_shader.use();
    m_shader.setMat4("u_Projection", m_projection);
    m_shader.setMat4("u_Model", model);
    m_shader.setVec4("u_Color", color);
    m_shader.setVec4("u_Tint", m_currentTint);
    m_shader.setBool("u_Grayscale", m_currentGrayscale);

    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        m_shader.setInt("u_Texture", 0);
        m_shader.setBool("u_UseTexture", true);
    } else {
        m_shader.setBool("u_UseTexture", false);
    }

    glBindVertexArray(m_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::drawQuadCentered(float x, float y, float width, float height, const glm::vec4& color, unsigned int textureID) {
    drawQuad(x - width * 0.5f, y - height * 0.5f, width, height, color, textureID);
}

void Renderer::drawBorder(float x, float y, float width, float height, float thickness, const glm::vec4& color) {
    // Top, bottom, left, right edges
    drawQuad(x, y, width, thickness, color);
    drawQuad(x, y + height - thickness, width, thickness, color);
    drawQuad(x, y, thickness, height, color);
    drawQuad(x + width - thickness, y, thickness, height, color);
}

// 5x7 ASCII bitmap font table (ASCII 32 ' ' to 90 'Z')
static const uint8_t FONT_5X7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32 ' '
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 33 '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34 '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // 35 '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // 36 '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37 '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 38 '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 39 '\''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // 40 '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // 41 ')'
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // 42 '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // 43 '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 44 ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45 '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 46 '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47 '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 48 '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 49 '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 50 '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 51 '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 52 '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53 '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 54 '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 55 '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56 '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 57 '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 58 ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 59 ';'
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 60 '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61 '='
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 62 '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 63 '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // 64 '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 65 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 66 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 67 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 68 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 69 'E'
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 70 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 71 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 72 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 73 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 74 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 75 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 76 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 77 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 78 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 79 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 80 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 81 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 82 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 83 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 84 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 85 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 86 'V'
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 87 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88 'X'
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 89 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 90 'Z'
};

float Renderer::getTextWidth(const std::string& text, float scale) {
    return text.length() * (6.0f * scale);
}

void Renderer::drawText(const std::string& text, float x, float y, float scale, const glm::vec4& color) {
    float curX = x;
    for (char c : text) {
        char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (upper >= 32 && upper <= 90) {
            int idx = upper - 32;
            const uint8_t* cols = FONT_5X7[idx];
            for (int col = 0; col < 5; ++col) {
                uint8_t line = cols[col];
                for (int row = 0; row < 7; ++row) {
                    if ((line >> row) & 1) {
                        drawQuad(curX + col * scale, y + row * scale, scale, scale, color);
                    }
                }
            }
        }
        curX += 6.0f * scale; // 5 cols + 1 space
    }
}

void Renderer::drawTextCentered(const std::string& text, float centerX, float centerY, float scale, const glm::vec4& color) {
    float width = getTextWidth(text, scale);
    float height = 7.0f * scale;
    drawText(text, centerX - width * 0.5f, centerY - height * 0.5f, scale, color);
}
