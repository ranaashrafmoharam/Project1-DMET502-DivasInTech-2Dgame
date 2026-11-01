#include "HierarchicalModel.h"
#include "../core/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

void HierarchicalModel::draw(float x, float y, float scale, bool flipX) {
    Renderer& ren = Renderer::instance();
    glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    if (flipX) {
        base = glm::scale(base, glm::vec3(-scale, scale, 1.0f));
    } else {
        base = glm::scale(base, glm::vec3(scale, scale, 1.0f));
    }

    ren.pushTransform(base);
    drawPart(m_root, base);
    ren.popTransform();
}

void HierarchicalModel::drawPart(const ModelPart& part, const glm::mat4& parentTransform) {
    Renderer& ren = Renderer::instance();
    glm::mat4 current = glm::translate(parentTransform, glm::vec3(part.offset, 0.0f));
    current = glm::rotate(current, part.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    current = glm::scale(current, glm::vec3(part.scale, 1.0f));

    // Render part rectangle
    ren.drawRect(-part.scale.x * 0.5f, -part.scale.y * 0.5f, part.scale.x, part.scale.y, part.color);

    for (const auto& child : part.children) {
        drawPart(child, current);
    }
}
