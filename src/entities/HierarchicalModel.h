#pragma once
// ====================================================================
// FILE:     src/entities/HierarchicalModel.h
// Hierarchical model system supporting parent-child transforms.
// ====================================================================

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct ModelPart {
    std::string name;
    glm::vec2 offset{0.0f, 0.0f};
    float rotation = 0.0f;
    glm::vec2 scale{1.0f, 1.0f};
    glm::vec4 color{1.0f};
    std::vector<ModelPart> children;
};

class HierarchicalModel {
public:
    HierarchicalModel() = default;

    void setRoot(const ModelPart& root) { m_root = root; }
    void draw(float x, float y, float scale = 1.0f, bool flipX = false);

private:
    void drawPart(const ModelPart& part, const glm::mat4& parentTransform);

    ModelPart m_root;
};
