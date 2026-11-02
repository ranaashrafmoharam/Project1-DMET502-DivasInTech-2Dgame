#pragma once
// ====================================================================
// FILE:     src/core/Texture.h
// OWNER:    Member 3
// TASKS:    M3-14
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
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
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - load(path)
//   - bind(slot)
//
// INTERACTS WITH: Scene, Mesh, HUD, PlayerCards
// ====================================================================

#pragma once

#include <string>

class Texture
{
public:
    static unsigned int load(const std::string& path);
    static void bind(unsigned int texture, unsigned int slot = 0);
    static void clearCache();
};
