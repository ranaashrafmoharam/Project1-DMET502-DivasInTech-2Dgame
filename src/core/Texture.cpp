#include "Texture.h"
// ====================================================================
// FILE:     src/core/Texture.cpp
// OWNER:    Member 3
// TASKS:    M3-14
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Loads images into OpenGL textures and caches them so the same
//   image is never loaded twice.
//
// RESPONSIBILITIES:
//   - Load an image file into a texture
//   - Cache textures by path
//   - Bind a texture for drawing
//
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - load(path)
//   - bind(slot)
//
// INTERACTS WITH: Scene, Mesh, HUD, PlayerCards
// ====================================================================

#include "Texture.h"
#include <glad/glad.h>
#include <map>
#include <fstream>
#include <vector>
#include <cstring>

namespace
{
std::map<std::string, unsigned int> cache;

unsigned int createTexture(unsigned char* pixels, int width, int height, bool rgba)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, rgba ? GL_RGBA : GL_RGB, width, height, 0,
                 rgba ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

// Load the BMP format supplied by tools such as Paint/GIMP; PNG/JPG can be
// added later through the project's stb_image dependency without changing API.
bool loadBmp(const std::string& path, std::vector<unsigned char>& pixels, int& width, int& height)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return false;
    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);
    if (header[0] != 'B' || header[1] != 'M')
        return false;
    width = header[18] | header[19] << 8 | header[20] << 16 | header[21] << 24;
    height = header[22] | header[23] << 8 | header[24] << 16 | header[25] << 24;
    int planes = header[26] | header[27] << 8;
    int bits = header[28] | header[29] << 8;
    if (planes != 1 || (bits != 24 && bits != 32) || width <= 0 || height <= 0)
        return false;
    int offset = header[10] | header[11] << 8 | header[12] << 16 | header[13] << 24;
    file.seekg(offset, std::ios::beg);
    int channels = bits / 8;
    int stride = width * channels;
    int rowPadding = stride % 4 ? 4 - stride % 4 : 0;
    std::vector<unsigned char> row(static_cast<size_t>(width) * channels);
    pixels.resize(static_cast<size_t>(width) * height * 4);
    for (int y = 0; y < height; ++y)
    {
        file.read(reinterpret_cast<char*>(row.data()), row.size());
        if (!file)
            return false;
        for (int x = 0; x < width; ++x)
        {
            size_t sourceIndex = static_cast<size_t>(x) * channels;
            size_t targetIndex = (static_cast<size_t>(y) * width + x) * 4;
            pixels[targetIndex + 0] = row[sourceIndex + 2];
            pixels[targetIndex + 1] = row[sourceIndex + 1];
            pixels[targetIndex + 2] = row[sourceIndex + 0];
            pixels[targetIndex + 3] = channels == 4 ? row[sourceIndex + 3] : 255;
        }
        file.seekg(rowPadding, std::ios::cur);
    }
    return true;
}
}

unsigned int Texture::load(const std::string& path)
{
    auto found = cache.find(path);
    if (found != cache.end())
        return found->second;

    std::vector<unsigned char> pixels;
    int width = 1, height = 1;
    unsigned int texture = 0;
    if (loadBmp(path, pixels, width, height))
        texture = createTexture(pixels.data(), width, height, true);
    else
    {
        unsigned char placeholder[4] = {255, 255, 255, 255};
        texture = createTexture(placeholder, 1, 1, true);
    }
    cache[path] = texture;
    return texture;
}

void Texture::bind(unsigned int texture, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::clearCache()
{
    for (const auto& entry : cache)
        glDeleteTextures(1, &entry.second);
    cache.clear();
}
