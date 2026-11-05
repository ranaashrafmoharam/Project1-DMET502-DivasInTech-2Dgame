#include "Button.h"
#include "../core/Renderer.h"
#include "TextRenderer.h"

Button::Button(const std::string& label, float x, float y, float w, float h, float fontScale)
    : m_label(label), m_x(x), m_y(y), m_w(w), m_h(h), m_fontScale(fontScale) {}

void Button::setBounds(float x, float y, float w, float h) {
    m_x = x; m_y = y; m_w = w; m_h = h;
}

void Button::setLabel(const std::string& label) {
    m_label = label;
}

void Button::update(glm::vec2 mousePos, bool leftClicked) {
    m_hovered = (mousePos.x >= m_x && mousePos.x <= m_x + m_w &&
                 mousePos.y >= m_y && mousePos.y <= m_y + m_h);
    m_clicked = (m_hovered && leftClicked);
}

void Button::draw() const {
    Renderer& ren = Renderer::instance();
    TextRenderer& tr = TextRenderer::instance();

    // Pokemon style button box:
    // Outer shadow
    ren.drawRect(m_x + 4, m_y - 4, m_w, m_h, glm::vec4(0.03f, 0.03f, 0.07f, 0.9f));

    // Outer border (accentuates when hovered)
    glm::vec4 borderColor = m_hovered ? m_accentColor + glm::vec4(0.12f, 0.12f, 0.12f, 0.0f)
                                      : m_accentColor;
    ren.drawRect(m_x - 3, m_y - 3, m_w + 6, m_h + 6, borderColor);

    // Inner background
    glm::vec4 bgColor = m_hovered ? glm::vec4(0.16f, 0.16f, 0.26f, 1.0f)
                                  : glm::vec4(0.07f, 0.08f, 0.14f, 1.0f);
    ren.drawRect(m_x, m_y, m_w, m_h, bgColor);

    // Inner thin border
    ren.drawRectOutline(m_x + 3, m_y + 3, m_w - 6, m_h - 6, 2.0f, 
                        m_hovered ? m_accentColor
                                  : glm::vec4(0.32f, 0.38f, 0.50f, 1.0f));

    // Pokemon style cursor arrow '>' on hover
    if (m_hovered) {
        tr.drawTextWithShadow(">", m_x + 14.0f, m_y + (m_h - tr.getTextHeight(m_fontScale)) * 0.5f,
                              m_fontScale, m_accentColor);
    }

    // Centered label with shadow
    float textY = m_y + (m_h - tr.getTextHeight(m_fontScale)) * 0.5f;
    glm::vec4 textCol = m_hovered ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
                                  : glm::vec4(0.85f, 0.90f, 0.95f, 1.0f);
    tr.drawTextCenteredWithShadow(m_label, m_x + m_w * 0.5f, textY, m_fontScale, textCol);
}
