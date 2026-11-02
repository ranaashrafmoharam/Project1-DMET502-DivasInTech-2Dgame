#include "Scene.h"
// ====================================================================
// FILE:     src/scene/Scene.cpp
// OWNER:    Member 3
// TASKS:    M3-02, US-16, US-51
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Draws the whole game world behind the units in classic Game Boy /
//   retro Pokemon pixel art aesthetic.
//
// RESPONSIBILITIES:
//   - Layered dusk sky, moon, stars, parallax mountains and pagoda silhouettes
//   - 3 distinct battle lanes with stone pavers, dividers, and ground details
//   - Seamless integration with HomeBase and EnvironmentElements
//   - High performance modern OpenGL 3.3 batched pixel renderer
// ====================================================================

#include <cmath>
#include <vector>
#include <fstream>
#include <map>
#include <glad/glad.h>
#include "M3Font.h"

namespace
{
GLuint program = 0;
GLuint vao = 0;
GLuint vbo = 0;
GLint projectionLocation = -1;
float width = 1280.0f;
float height = 720.0f;
float sceneTime = 0.0f;

// Batched vertex buffer: x, y, r, g, b, a (6 floats per vertex)
constexpr size_t MAX_VERTICES = 48000;
std::vector<float> batchVertices;
GLuint textureProgram = 0;
GLuint textureVao = 0;
GLuint textureVbo = 0;
GLint textureProjectionLocation = -1;
std::map<std::string, GLuint> imageCache;

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

void initializeRenderer()
{
    if (program != 0)
        return;

    const char* vertexSource =
        "#version 330 core\n"
        "layout(location = 0) in vec2 aPosition;\n"
        "layout(location = 1) in vec4 aColour;\n"
        "uniform mat4 uProjection;\n"
        "out vec4 vColour;\n"
        "void main() {\n"
        "    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);\n"
        "    vColour = aColour;\n"
        "}\n";

    const char* fragmentSource =
        "#version 330 core\n"
        "in vec4 vColour;\n"
        "out vec4 FragColour;\n"
        "void main() {\n"
        "    FragColour = vColour;\n"
        "}\n";

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    projectionLocation = glGetUniformLocation(program, "uProjection");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertex format: [pos.x, pos.y, col.r, col.g, col.b, col.a]
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    batchVertices.reserve(MAX_VERTICES * 6);
}

void setProjection()
{
    const float projection[16] = {
        2.0f / width, 0.0f,          0.0f,  0.0f,
        0.0f,        -2.0f / height, 0.0f,  0.0f,
        0.0f,         0.0f,         -1.0f,  0.0f,
       -1.0f,         1.0f,          0.0f,  1.0f
    };
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
}

GLuint loadBmpTexture(const char* path)
{
    auto cached = imageCache.find(path);
    if (cached != imageCache.end())
        return cached->second;

    std::ifstream file(path, std::ios::binary);
    if (!file)
        return 0;
    unsigned char header[54]{};
    file.read(reinterpret_cast<char*>(header), sizeof(header));
    if (header[0] != 'B' || header[1] != 'M')
        return 0;

    int imageWidth = *reinterpret_cast<int*>(&header[18]);
    int imageHeight = *reinterpret_cast<int*>(&header[22]);
    int bits = header[28] | (header[29] << 8);
    int offset = *reinterpret_cast<int*>(&header[10]);
    if (imageWidth <= 0 || imageHeight <= 0 || (bits != 24 && bits != 32))
        return 0;

    const int channels = bits / 8;
    const int rowBytes = ((imageWidth * channels + 3) / 4) * 4;
    std::vector<unsigned char> row(rowBytes);
    std::vector<unsigned char> pixels(static_cast<size_t>(imageWidth) * imageHeight * 4);
    file.seekg(offset, std::ios::beg);
    for (int y = 0; y < imageHeight; ++y)
    {
        file.read(reinterpret_cast<char*>(row.data()), rowBytes);
        int targetY = imageHeight - 1 - y;
        for (int x = 0; x < imageWidth; ++x)
        {
            const size_t source = static_cast<size_t>(x) * channels;
            const size_t target = (static_cast<size_t>(targetY) * imageWidth + x) * 4;
            pixels[target + 0] = row[source + 2];
            pixels[target + 1] = row[source + 1];
            pixels[target + 2] = row[source + 0];
            pixels[target + 3] = channels == 4 ? row[source + 3] : 255;
        }
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Keep uploaded pixel-art sprites crisp and consistent with the
    // procedural character sprites instead of smoothing their silhouettes.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    imageCache[path] = texture;
    return texture;
}

void initializeTextureRenderer()
{
    if (textureProgram != 0)
        return;
    const char* vertexSource =
        "#version 330 core\n"
        "layout(location = 0) in vec2 aPosition;\n"
        "layout(location = 1) in vec2 aUv;\n"
        "layout(location = 2) in vec4 aColour;\n"
        "uniform mat4 uProjection;\n"
        "out vec2 vUv; out vec4 vColour;\n"
        "void main(){gl_Position=uProjection*vec4(aPosition,0.0,1.0);vUv=aUv;vColour=aColour;}\n";
    const char* fragmentSource =
        "#version 330 core\n"
        "in vec2 vUv; in vec4 vColour; uniform sampler2D uTexture; out vec4 FragColour;\n"
        "void main(){FragColour=texture(uTexture,vUv)*vColour;}\n";
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    textureProgram = glCreateProgram();
    glAttachShader(textureProgram, vs); glAttachShader(textureProgram, fs);
    glLinkProgram(textureProgram);
    glDeleteShader(vs); glDeleteShader(fs);
    textureProjectionLocation = glGetUniformLocation(textureProgram, "uProjection");
    glGenVertexArrays(1, &textureVao);
    glGenBuffers(1, &textureVbo);
    glBindVertexArray(textureVao);
    glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(4 * sizeof(float)));
}

inline void pushVertex(float x, float y, float r, float g, float b, float a)
{
    batchVertices.push_back(x);
    batchVertices.push_back(y);
    batchVertices.push_back(r);
    batchVertices.push_back(g);
    batchVertices.push_back(b);
    batchVertices.push_back(a);
}

// Falling cherry blossom petals
struct Petal
{
    float baseX;
    float startY;
    float speedY;
    float swayAmp;
    float swayFreq;
    float phase;
    float size;
};

std::vector<Petal> petals;
void initPetals()
{
    if (!petals.empty())
        return;
    for (int i = 0; i < 45; ++i)
    {
        Petal p;
        p.baseX = std::fmod(i * 37.0f * 13.0f, 1360.0f) - 40.0f;
        p.startY = std::fmod(i * 53.0f * 17.0f, 760.0f) - 20.0f;
        p.speedY = 28.0f + std::fmod(i * 11.0f, 26.0f);
        p.swayAmp = 15.0f + std::fmod(i * 7.0f, 22.0f);
        p.swayFreq = 1.2f + std::fmod(i * 3.0f, 1.8f);
        p.phase = i * 0.8f;
        p.size = 3.5f + std::fmod(i * 2.0f, 3.0f);
        petals.push_back(p);
    }
}
}

void M3Draw::init(float screenWidth, float screenHeight)
{
    width = screenWidth;
    height = screenHeight;
    initializeRenderer();
    initPetals();
}

float M3Draw::getTime()
{
    return sceneTime;
}

void M3Draw::advanceTime(float dt)
{
    sceneTime += dt;
}

void M3Draw::flush()
{
    if (batchVertices.empty())
        return;

    initializeRenderer();
    glUseProgram(program);
    setProjection();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, batchVertices.size() * sizeof(float), batchVertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(batchVertices.size() / 6));

    batchVertices.clear();
}

void M3Draw::quad(float x, float y, float w, float h,
                  float r, float g, float b, float a)
{
    if (batchVertices.size() + 36 >= MAX_VERTICES * 6)
        flush();

    float x2 = x + w;
    float y2 = y + h;

    // Triangle 1
    pushVertex(x, y, r, g, b, a);
    pushVertex(x2, y, r, g, b, a);
    pushVertex(x2, y2, r, g, b, a);

    // Triangle 2
    pushVertex(x, y, r, g, b, a);
    pushVertex(x2, y2, r, g, b, a);
    pushVertex(x, y2, r, g, b, a);
}

void M3Draw::image(const char* path, float x, float y, float w, float h, float alpha)
{
    imageRegion(path, x, y, w, h, 0.0f, 0.0f, 1.0f, 1.0f, alpha);
}

void M3Draw::imageRegion(const char* path, float x, float y, float w, float h,
                         float u0, float v0, float u1, float v1, float alpha)
{
    if (!path)
        return;
    GLuint texture = loadBmpTexture(path);
    if (texture == 0)
        return;

    // Keep image ordering correct relative to the coloured primitive batch.
    flush();
    initializeTextureRenderer();
    const float vertices[] = {
        x,     y,     u0, v0, 1.0f, 1.0f, 1.0f, alpha,
        x + w, y,     u1, v0, 1.0f, 1.0f, 1.0f, alpha,
        x + w, y + h, u1, v1, 1.0f, 1.0f, 1.0f, alpha,
        x,     y,     u0, v0, 1.0f, 1.0f, 1.0f, alpha,
        x + w, y + h, u1, v1, 1.0f, 1.0f, 1.0f, alpha,
        x,     y + h, u0, v1, 1.0f, 1.0f, 1.0f, alpha
    };
    const float projection[16] = {
        2.0f / width, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / height, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    glUseProgram(textureProgram);
    glUniformMatrix4fv(textureProjectionLocation, 1, GL_FALSE, projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(textureProgram, "uTexture"), 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(textureVao);
    glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void M3Draw::circle(float cx, float cy, float radius,
                    float r, float g, float b, float a, int segments)
{
    if (segments < 3) segments = 3;
    if (batchVertices.size() + static_cast<size_t>(segments) * 18 >= MAX_VERTICES * 6)
        flush();

    float angleStep = 6.283185307f / segments;
    for (int i = 0; i < segments; ++i)
    {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        float x1 = cx + std::cos(a1) * radius;
        float y1 = cy + std::sin(a1) * radius;
        float x2 = cx + std::cos(a2) * radius;
        float y2 = cy + std::sin(a2) * radius;

        pushVertex(cx, cy, r, g, b, a);
        pushVertex(x1, y1, r, g, b, a);
        pushVertex(x2, y2, r, g, b, a);
    }
}

void M3Draw::triangle(float x1, float y1, float x2, float y2, float x3, float y3,
                      float r, float g, float b, float a)
{
    if (batchVertices.size() + 18 >= MAX_VERTICES * 6)
        flush();

    pushVertex(x1, y1, r, g, b, a);
    pushVertex(x2, y2, r, g, b, a);
    pushVertex(x3, y3, r, g, b, a);
}

void M3Draw::line(float x1, float y1, float x2, float y2, float thickness,
                  float r, float g, float b, float a)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);
    if (length < 0.001f)
        return;

    float nx = -dy / length * thickness * 0.5f;
    float ny =  dx / length * thickness * 0.5f;

    if (batchVertices.size() + 36 >= MAX_VERTICES * 6)
        flush();

    pushVertex(x1 + nx, y1 + ny, r, g, b, a);
    pushVertex(x1 - nx, y1 - ny, r, g, b, a);
    pushVertex(x2 - nx, y2 - ny, r, g, b, a);

    pushVertex(x1 + nx, y1 + ny, r, g, b, a);
    pushVertex(x2 - nx, y2 - ny, r, g, b, a);
    pushVertex(x2 + nx, y2 + ny, r, g, b, a);
}

void M3Draw::text(float x, float y, const char* str, float scale,
                  float r, float g, float b, float a)
{
    if (!str) return;

    float curX = x;
    float curY = y;
    float startX = x;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(str);
    while (*p)
    {
        // Handle UTF-8 curly double quotes: e2 80 9c, e2 80 9d
        if (p[0] == 0xE2 && p[1] == 0x80 && (p[2] == 0x9C || p[2] == 0x9D))
        {
            p += 3;
            // Draw regular double quote
            const uint8_t* col = M3Font::GLYPHS['"' - 32];
            for (int c = 0; c < 5; ++c)
            {
                uint8_t bits = col[c];
                for (int row = 0; row < 7; ++row)
                {
                    if (bits & (1 << row))
                        quad(curX + c * scale, curY + row * scale, scale, scale, r, g, b, a);
                }
            }
            curX += 6.0f * scale;
            continue;
        }

        // Handle UTF-8 curly single quote / apostrophe: e2 80 98, e2 80 99
        if (p[0] == 0xE2 && p[1] == 0x80 && (p[2] == 0x98 || p[2] == 0x99))
        {
            p += 3;
            const uint8_t* col = M3Font::GLYPHS['\'' - 32];
            for (int c = 0; c < 5; ++c)
            {
                uint8_t bits = col[c];
                for (int row = 0; row < 7; ++row)
                {
                    if (bits & (1 << row))
                        quad(curX + c * scale, curY + row * scale, scale, scale, r, g, b, a);
                }
            }
            curX += 6.0f * scale;
            continue;
        }

        char ch = *p++;
        if (ch == '\n')
        {
            curX = startX;
            curY += 9.0f * scale;
            continue;
        }

        if (ch == ' ')
        {
            curX += 4.5f * scale;
            continue;
        }

        if (ch < 32 || ch > 126)
            continue;

        const uint8_t* col = M3Font::GLYPHS[ch - 32];
        for (int c = 0; c < 5; ++c)
        {
            uint8_t bits = col[c];
            for (int row = 0; row < 7; ++row)
            {
                if (bits & (1 << row))
                {
                    quad(curX + c * scale, curY + row * scale, scale, scale, r, g, b, a);
                }
            }
        }
        curX += 6.0f * scale;
    }
}

float M3Draw::textWidth(const char* str, float scale)
{
    if (!str) return 0.0f;
    float w = 0.0f;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(str);
    while (*p)
    {
        if (p[0] == 0xE2 && p[1] == 0x80 && (p[2] == 0x9C || p[2] == 0x9D || p[2] == 0x98 || p[2] == 0x99))
        {
            p += 3;
            w += 6.0f * scale;
            continue;
        }
        char ch = *p++;
        if (ch == ' ')
            w += 4.5f * scale;
        else if (ch >= 32 && ch <= 126)
            w += 6.0f * scale;
    }
    return w;
}

void Scene::init()
{
    M3Draw::init();
}

void Scene::draw(bool includeLanterns, bool includeLeftLantern)
{
    // =========================================================================
    // 1. DUSK SKY & STARS (Game Boy / Pokemon 16-bit aesthetic)
    // =========================================================================
    // Stepped gradient dither bands from deep night sky to warm dusk horizon
    M3Draw::quad(0,   0, 1280,  90, 0.08f, 0.07f, 0.17f);
    M3Draw::quad(0,  90, 1280,  80, 0.13f, 0.10f, 0.24f);
    M3Draw::quad(0, 170, 1280,  80, 0.20f, 0.14f, 0.31f);
    M3Draw::quad(0, 250, 1280,  70, 0.32f, 0.18f, 0.36f);
    M3Draw::quad(0, 320, 1280,  70, 0.48f, 0.26f, 0.36f);

    // Subtle horizon glow line
    M3Draw::quad(0, 385, 1280,   5, 0.70f, 0.36f, 0.35f, 0.5f);

    // Twinkling Pixel Stars (fixed seeds)
    const float starCoords[][2] = {
        {60, 42}, {140, 75}, {220, 30}, {310, 85}, {400, 25}, {480, 65},
        {560, 40}, {630, 95}, {720, 35}, {810, 80}, {900, 50}, {990, 20},
        {1070, 70}, {1180, 45}, {1240, 85}, {180, 130}, {350, 150},
        {520, 120}, {700, 140}, {860, 115}, {1020, 135}, {1140, 120}
    };
    for (size_t i = 0; i < sizeof(starCoords)/sizeof(starCoords[0]); ++i)
    {
        float twinkle = 0.65f + 0.35f * std::sin(M3Draw::getTime() * 3.0f + i * 1.3f);
        float sx = starCoords[i][0];
        float sy = starCoords[i][1];
        M3Draw::quad(sx, sy, 3, 3, 0.95f, 0.94f, 0.85f, twinkle);
        if (i % 3 == 0)
        {
            M3Draw::quad(sx - 2, sy + 1, 7, 1, 1.0f, 0.95f, 0.75f, twinkle * 0.7f);
            M3Draw::quad(sx + 1, sy - 2, 1, 7, 1.0f, 0.95f, 0.75f, twinkle * 0.7f);
        }
    }

    // =========================================================================
    // 2. LUMINOUS PIXEL MOON & CLOUDS
    // =========================================================================
    // Moon halo
    M3Draw::circle(1110, 145, 62, 0.96f, 0.88f, 0.55f, 0.18f, 32);
    M3Draw::circle(1110, 145, 48, 0.98f, 0.92f, 0.62f, 0.35f, 32);
    // Moon core
    M3Draw::circle(1110, 145, 36, 0.99f, 0.95f, 0.74f, 1.0f, 28);
    // Moon crater pixel spots
    M3Draw::circle(1100, 137,  7, 0.88f, 0.82f, 0.62f, 0.8f, 12);
    M3Draw::circle(1122, 153,  5, 0.88f, 0.82f, 0.62f, 0.8f, 12);
    M3Draw::circle(1114, 131,  4, 0.88f, 0.82f, 0.62f, 0.7f, 10);

    // Drifting retro clouds (layered rounded rectangles)
    auto drawCloud = [](float cx, float cy, float w, float h, float alpha) {
        M3Draw::quad(cx, cy, w, h, 0.35f, 0.22f, 0.38f, alpha);
        M3Draw::quad(cx + w * 0.15f, cy - h * 0.45f, w * 0.65f, h * 0.45f, 0.35f, 0.22f, 0.38f, alpha);
        M3Draw::quad(cx + w * 0.30f, cy - h * 0.75f, w * 0.35f, h * 0.35f, 0.35f, 0.22f, 0.38f, alpha);
    };
    drawCloud(80,  180, 220, 26, 0.65f);
    drawCloud(460, 210, 260, 28, 0.55f);
    drawCloud(820, 160, 280, 30, 0.60f);

    // =========================================================================
    // 3. PARALLAX MOUNTAINS & DISTANT PAGODAS (Ninjago Mountains)
    // =========================================================================
    // Layer 1: Distant misty peaks
    for (int p = 0; p < 8; ++p)
    {
        float mx = p * 180.0f - 40.0f;
        M3Draw::triangle(mx, 390, mx + 95, 230, mx + 190, 390, 0.16f, 0.14f, 0.26f, 0.85f);
    }

    // Layer 2: Midground rocky mountains with snow caps
    for (int p = 0; p < 7; ++p)
    {
        float mx = p * 210.0f - 60.0f;
        float peakY = 270.0f + (p % 3) * 25.0f;
        M3Draw::triangle(mx, 390, mx + 105, peakY, mx + 210, 390, 0.11f, 0.10f, 0.20f, 1.0f);
        // Snow/rock highlight on peak
        M3Draw::triangle(mx + 85, peakY + 35, mx + 105, peakY, mx + 125, peakY + 35, 0.75f, 0.70f, 0.82f, 0.4f);
    }

    // Distant Pagoda silhouettes on mountain peaks (adds depth and Japanese theme)
    auto drawDistantPagoda = [](float px, float py) {
        // Base
        M3Draw::quad(px - 14, py, 28, 30, 0.08f, 0.08f, 0.15f);
        // Tier 1 roof
        M3Draw::quad(px - 22, py - 4, 44, 6, 0.08f, 0.08f, 0.15f);
        // Tier 2
        M3Draw::quad(px - 10, py - 16, 20, 12, 0.08f, 0.08f, 0.15f);
        M3Draw::quad(px - 16, py - 20, 32, 5, 0.08f, 0.08f, 0.15f);
        // Spire
        M3Draw::line(px, py - 20, px, py - 32, 2.0f, 0.08f, 0.08f, 0.15f);
    };
    drawDistantPagoda(320, 340);
    drawDistantPagoda(780, 325);

    // =========================================================================
    // 4. BATTLE LANES & STONE PAVEMENT (3 Distinct Lanes)
    // =========================================================================
    const float laneTop = 390.0f;
    const float laneHeight = 110.0f;

    // Outer playfield ground foundation
    M3Draw::quad(0, laneTop, 1280, 330, 0.22f, 0.20f, 0.19f);

    for (int lane = 0; lane < 3; ++lane)
    {
        float y = laneTop + lane * laneHeight;

        // Lane base tone (alternating subtle tone for clear contrast)
        float baseR = lane % 2 == 0 ? 0.30f : 0.27f;
        float baseG = lane % 2 == 0 ? 0.28f : 0.25f;
        float baseB = lane % 2 == 0 ? 0.25f : 0.22f;
        M3Draw::quad(0, y, 1280, laneHeight, baseR, baseG, baseB);

        // Stone flagstone tiles pattern (authentic retro RPG pavement)
        for (int tile = 0; tile < 32; ++tile)
        {
            float tx = tile * 42.0f + (lane * 14.0f);
            float ty = y + 12.0f + (tile % 4) * 22.0f;

            // Paver body
            M3Draw::quad(tx, ty, 34.0f, 16.0f, baseR + 0.05f, baseG + 0.05f, baseB + 0.04f);
            // Highlight edge (top/left)
            M3Draw::quad(tx, ty, 34.0f, 2.0f, baseR + 0.12f, baseG + 0.12f, baseB + 0.10f);
            M3Draw::quad(tx, ty, 2.0f, 16.0f, baseR + 0.12f, baseG + 0.12f, baseB + 0.10f);
            // Shadow edge (bottom/right)
            M3Draw::quad(tx, ty + 14.0f, 34.0f, 2.0f, baseR - 0.08f, baseG - 0.08f, baseB - 0.07f);
            M3Draw::quad(tx + 32.0f, ty, 2.0f, 16.0f, baseR - 0.08f, baseG - 0.08f, baseB - 0.07f);

            // Occasional moss tufts in crevices
            if ((tile + lane) % 5 == 0)
            {
                M3Draw::quad(tx + 4, ty + 12, 6, 3, 0.20f, 0.40f, 0.18f);
            }
        }

        // Heavy dark wood lane separator beam
        M3Draw::quad(0, y, 1280, 5, 0.14f, 0.10f, 0.08f);
        // Golden brass trim on the lane divider
        M3Draw::quad(0, y + 1, 1280, 2, 0.74f, 0.58f, 0.28f);

        // Lane label on far right edge (faint decorative marker)
        char laneBuf[16];
        snprintf(laneBuf, sizeof(laneBuf), "LANE %d", lane + 1);
        M3Draw::text(1210, y + 48, laneBuf, 1.4f, 0.65f, 0.55f, 0.35f, 0.4f);
    }

    // Bottom boundary beam
    M3Draw::quad(0, laneTop + laneHeight * 3 - 4, 1280, 6, 0.14f, 0.10f, 0.08f);
    M3Draw::quad(0, laneTop + laneHeight * 3 - 2, 1280, 2, 0.74f, 0.58f, 0.28f);

    // =========================================================================
    // 5. CALL ENVIRONMENT ELEMENTS & HOME BASE
    // =========================================================================
    EnvironmentElements environment;
    environment.drawElement1(); // Torii gate (right spawn portal)
    environment.drawElement2(); // Multi-tier monastery pagoda
    environment.drawElement3(); // Ninjago 4 clan banners (Fire, Lightning, Ice, Earth)
    if (includeLanterns)
        environment.drawElement4(includeLeftLantern); // Stone lanterns with glowing fire
    environment.drawElement5(); // Martial arts training dummy & weapon stand
    environment.drawElement6(); // Ancient Sakura cherry blossom tree

    HomeBase home;
    home.draw(); // Monastery Gate (safe zone & losing line)

    // =========================================================================
    // 6. AMBIENT FALLING SAKURA CHERRY PETALS
    // =========================================================================
    float t = M3Draw::getTime();
    for (size_t i = 0; i < petals.size(); ++i)
    {
        const Petal& p = petals[i];
        float curY = std::fmod(p.startY + t * p.speedY, 740.0f) - 10.0f;
        float curX = p.baseX + std::sin(t * p.swayFreq + p.phase) * p.swayAmp - (t * 18.0f);
        curX = std::fmod(curX + 1400.0f, 1360.0f) - 40.0f;

        // Pink/rose gradient cherry petal
        float pr = 0.96f - (i % 3) * 0.06f;
        float pg = 0.68f + (i % 2) * 0.10f;
        float pb = 0.78f;
        M3Draw::quad(curX, curY, p.size, p.size * 0.75f, pr, pg, pb, 0.85f);
        M3Draw::quad(curX + 1, curY + 1, p.size * 0.6f, p.size * 0.5f, 1.0f, 0.88f, 0.94f, 0.9f);
    }

    M3Draw::flush();
}
