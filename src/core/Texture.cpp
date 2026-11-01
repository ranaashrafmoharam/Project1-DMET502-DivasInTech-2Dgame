#include "Texture.h"
#include <fstream>
#include <vector>
#include <iostream>

TextureManager& TextureManager::instance() {
    static TextureManager mgr;
    return mgr;
}

TextureInfo TextureManager::createFromRGBA(const std::string& name, int width, int height, const unsigned char* data) {
    if (m_textures.find(name) != m_textures.end()) {
        return m_textures[name];
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // NEAREST filtering is crucial for authentic Game Boy / Pokemon pixel art!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    TextureInfo info{tex, width, height};
    m_textures[name] = info;
    return info;
}

TextureInfo TextureManager::get(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second;
    }

    // Attempt to load from assets/sprites/<name>.bin
    std::string binPath = "assets/sprites/" + name + ".bin";
    std::ifstream file(binPath, std::ios::binary);
    if (file.is_open()) {
        uint32_t w = 0, h = 0;
        file.read(reinterpret_cast<char*>(&w), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&h), sizeof(uint32_t));
        if (w > 0 && h > 0 && w < 4096 && h < 4096) {
            std::vector<unsigned char> buffer(w * h * 4);
            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            return createFromRGBA(name, static_cast<int>(w), static_cast<int>(h), buffer.data());
        }
    }

    std::cerr << "Could not find texture: " << name << " at " << binPath << std::endl;
    return TextureInfo{0, 0, 0};
}

GLuint TextureManager::getID(const std::string& name) {
    return get(name).id;
}

bool TextureManager::init() {
    const std::vector<std::string> preloadList = {
        "kai", "zane", "cole", "jay", "wu", "garmadon",
        "venomari", "hypnobrai", "constrictai",
        "shard", "sword", "shuriken", "acid", "sonic",
        "monastery_gate", "cherry_tree", "stone_lantern",
        "training_dummy", "prayer_gong", "dragon_banner"
    };

    for (const auto& s : preloadList) {
        get(s);
    }
    return true;
}

void TextureManager::cleanup() {
    for (auto& pair : m_textures) {
        if (pair.second.id != 0) {
            glDeleteTextures(1, &pair.second.id);
        }
    }
    m_textures.clear();
}
