#pragma once
// ====================================================================
// FILE:     src/core/Input.h
// OWNER:    Member 1
// TASKS:    M1-01, M1-04
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Wraps raw GLFW callbacks into a simple query API so game code
//   never touches GLFW directly.
//
// RESPONSIBILITIES:
//   - Track mouse position in world coordinates
//   - Track left-click events (pressed this frame)
//   - Track W / Up, S / Down and Esc presses
// ====================================================================

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input {
public:
    static void init(GLFWwindow* window);
    static void update(); // Call at the beginning of each frame to update input state

    static glm::vec2 getMouseWorldPos();
    static bool wasLeftClicked();
    static bool jumpPressed();
    static bool duckPressed();
    static bool escPressed();
    static bool pPressed();

    // Callbacks for GLFW
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    static glm::vec2 s_mousePos;
    static bool s_leftClickedThisFrame;
    static bool s_leftMouseDown;

    static bool s_jumpPressedThisFrame;
    static bool s_duckPressedThisFrame;
    static bool s_escPressedThisFrame;
    static bool s_pPressedThisFrame;
};
