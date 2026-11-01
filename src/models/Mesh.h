#pragma once
// ====================================================================
// FILE:     src/models/Mesh.h
// Lightweight mesh buffer representation.
// ====================================================================

#include <glad/glad.h>
#include <vector>

struct Vertex {
    float x, y, z;
    float u, v;
};

class Mesh {
public:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;

    Mesh() = default;
};
