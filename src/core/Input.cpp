// ====================================================================
// FILE:     src/core/Input.cpp
// OWNER:    Member 1
// TASKS:    M1-01, M1-04
// STATUS:   IMPLEMENTED
// ====================================================================

#include "Input.h"

glm::vec2 Input::s_mousePos = glm::vec2(0.0f);
bool Input::s_leftClickedThisFrame = false;
bool Input::s_leftMouseDown = false;
bool Input::s_jumpPressedThisFrame = false;
bool Input::s_duckPressedThisFrame = false;
bool Input::s_escPressedThisFrame = false;
bool Input::s_pPressedThisFrame = false;

void Input::init(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void Input::update() {
    // Reset single-frame trigger flags
    s_leftClickedThisFrame = false;
    s_jumpPressedThisFrame = false;
    s_duckPressedThisFrame = false;
    s_escPressedThisFrame = false;
    s_pPressedThisFrame = false;
}

glm::vec2 Input::getMouseWorldPos() {
    return s_mousePos;
}

bool Input::wasLeftClicked() {
    return s_leftClickedThisFrame;
}

bool Input::jumpPressed() {
    return s_jumpPressedThisFrame;
}

bool Input::duckPressed() {
    return s_duckPressedThisFrame;
}

bool Input::escPressed() {
    return s_escPressedThisFrame;
}

bool Input::pPressed() {
    return s_pPressedThisFrame;
}

void Input::cursorPosCallback(GLFWwindow* /*window*/, double xpos, double ypos) {
    s_mousePos.x = static_cast<float>(xpos);
    s_mousePos.y = static_cast<float>(ypos);
}

void Input::mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            s_leftClickedThisFrame = true;
            s_leftMouseDown = true;
        } else if (action == GLFW_RELEASE) {
            s_leftMouseDown = false;
        }
    }
}

void Input::keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            s_jumpPressedThisFrame = true;
        }
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            s_duckPressedThisFrame = true;
        }
        if (key == GLFW_KEY_ESCAPE) {
            s_escPressedThisFrame = true;
        }
        if (key == GLFW_KEY_P) {
            s_pPressedThisFrame = true;
        }
    }
}
