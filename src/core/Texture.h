#pragma once
// ====================================================================
// FILE:     src/core/Texture.h
// Loads images into OpenGL textures and caches them.
// ====================================================================

#include <glad/glad.h>
#include <string>
#include <unordered_map>

struct TextureInfo {
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

class TextureManager {
public:
    static TextureManager& instance();

    bool init();
    void cleanup();

    TextureInfo get(const std::string& name);
    GLuint getID(const std::string& name);

    // Register a texture from raw RGBA buffer
    TextureInfo createFromRGBA(const std::string& name, int width, int height, const unsigned char* data);

private:
    TextureManager() = default;
    ~TextureManager() = default;

    std::unordered_map<std::string, TextureInfo> m_textures;
};
