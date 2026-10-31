#include "Shader.h"
// ====================================================================
// FILE:     src/core/Shader.cpp
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   Loads, compiles and links shader programs from the
//   assets/shaders folder and lets other code set uniforms.
//
// RESPONSIBILITIES:
//   - Read vertex/fragment shader files
//   - Report compile and link errors
//   - Set uniforms (matrices, colour, tint, grayscale flag)
//
// PLANNED PUBLIC INTERFACE (names to agree on, implemented in this file):
//   - load(vertPath, fragPath)
//   - use()
//   - setMat4 / setVec4 / setFloat / setBool
//
// INTERACTS WITH: Renderer, TextRenderer
// ====================================================================
