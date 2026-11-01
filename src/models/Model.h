#pragma once
// ====================================================================
// FILE:     src/models/Model.h
// Optional Assimp loader.
// ====================================================================

#include "Mesh.h"
#include <vector>
#include <string>

class Model {
public:
    std::vector<Mesh> meshes;

    bool loadModel(const std::string&) { return true; }
    void draw() {}
};
