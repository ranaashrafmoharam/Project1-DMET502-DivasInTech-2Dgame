#pragma once
// ====================================================================
// FILE:     src/ui/Button.h
// OWNER:    Member 4
// TASKS:    M4-02
// Reusable clickable button with retro Pokemon styling and hover.
// ====================================================================

#include <glm/glm.hpp>
#include <string>

class Button {
public:
    Button() = default;
    Button(const std::string& label, float x, float y, float w, float h, float fontScale = 3.0f);

    void setBounds(float x, float y, float w, float h);
    void setLabel(const std::string& label);
    void setAccentColor(const glm::vec4& color) { m_accentColor = color; }

    void update(glm::vec2 mousePos, bool leftClicked);
    void draw() const;

    bool wasClicked() const { return m_clicked; }
    bool isHovered() const { return m_hovered; }

private:
    std::string m_label;
    float m_x = 0.0f;
    float m_y = 0.0f;
    float m_w = 0.0f;
    float m_h = 0.0f;
    float m_fontScale = 3.0f;
    glm::vec4 m_accentColor = glm::vec4(0.78f, 0.62f, 0.20f, 1.0f);

    bool m_hovered = false;
    bool m_clicked = false;
};
