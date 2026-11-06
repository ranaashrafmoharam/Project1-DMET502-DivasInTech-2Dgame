#pragma once
// ====================================================================
// FILE:     src/ui/TextRenderer.h
// OWNER:    Member 4
// TASKS:    M4-01
// Bitmap font renderer for Game Boy / Pokemon style retro text.
// ====================================================================

#include <glm/glm.hpp>
#include <string>

class TextRenderer {
public:
    static TextRenderer& instance();

    bool init();

    // Basic text drawing
    void drawText(const std::string& text, float x, float y, float scale = 2.0f, const glm::vec4& color = glm::vec4(1.0f));
    void drawTextCentered(const std::string& text, float cx, float y, float scale = 2.0f, const glm::vec4& color = glm::vec4(1.0f));

    // Pokemon style drop-shadow text
    void drawTextWithShadow(const std::string& text, float x, float y, float scale = 2.0f, 
                            const glm::vec4& color = glm::vec4(1.0f), 
                            const glm::vec4& shadowColor = glm::vec4(0.08f, 0.08f, 0.12f, 1.0f));
    void drawTextCenteredWithShadow(const std::string& text, float cx, float y, float scale = 2.0f, 
                                    const glm::vec4& color = glm::vec4(1.0f), 
                                    const glm::vec4& shadowColor = glm::vec4(0.08f, 0.08f, 0.12f, 1.0f));

    // Pokemon style dialogue frame with speaker badge
    void drawDialogueBox(const std::string& speaker, const std::string& line1, const std::string& line2,
                         float x, float y, float w, float h, float scale = 2.5f);

    float getTextWidth(const std::string& text, float scale = 2.0f) const;
    float getTextHeight(float scale = 2.0f) const;

private:
    TextRenderer() = default;
};
