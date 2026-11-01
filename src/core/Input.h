#pragma once
// ====================================================================
// FILE:     src/core/Input.h
// Wraps GLFW callbacks into queries: mouse position, clicks, keys.
// ====================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input {
public:
    static Input& instance();

    void init(GLFWwindow* window);
    void newFrame();

    // Queries
    glm::vec2 getMouseWorldPos() const { return m_mouseWorld; }
    bool wasLeftClicked() const { return m_leftClicked; }
    bool isLeftDown() const { return m_leftDown; }
    bool jumpPressed() const { return m_jumpPressed; }
    bool duckPressed() const { return m_duckPressed; }
    bool escPressed() const { return m_escPressed; }
    bool spacePressed() const { return m_spacePressed; }
    bool enterPressed() const { return m_enterPressed; }

    // GLFW callback handlers
    void onMouseButton(int button, int action);
    void onKey(int key, int action);
    void updateCursor(double xpos, double ypos, int winW, int winH);

private:
    Input() = default;

    glm::vec2 m_mouseWorld{0.0f, 0.0f};
    bool m_leftClicked = false;
    bool m_leftDown = false;

    bool m_jumpPressed = false;
    bool m_duckPressed = false;
    bool m_escPressed = false;
    bool m_spacePressed = false;
    bool m_enterPressed = false;
};
