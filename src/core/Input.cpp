#include "Input.h"
#include "Constants.h"

Input& Input::instance() {
    static Input inp;
    return inp;
}

void Input::init(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int) {
        Input::instance().onMouseButton(button, action);
    });

    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int action, int) {
        Input::instance().onKey(key, action);
    });
}

void Input::newFrame() {
    m_leftClicked = false;
    m_jumpPressed = false;
    m_duckPressed = false;
    m_escPressed = false;
    m_spacePressed = false;
    m_enterPressed = false;
}

void Input::updateCursor(double xpos, double ypos, int winW, int winH) {
    if (winW <= 0 || winH <= 0) return;
    // Map window pixels (origin top-left) to world coordinates (0..WINDOW_WIDTH, 0..WINDOW_HEIGHT, origin bottom-left)
    float wx = static_cast<float>(xpos * Constants::WINDOW_WIDTH / winW);
    float wy = static_cast<float>(Constants::WINDOW_HEIGHT - (ypos * Constants::WINDOW_HEIGHT / winH));
    m_mouseWorld = glm::vec2(wx, wy);
}

void Input::onMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_leftClicked = true;
            m_leftDown = true;
        } else if (action == GLFW_RELEASE) {
            m_leftDown = false;
        }
    }
}

void Input::onKey(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            m_jumpPressed = true;
        }
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            m_duckPressed = true;
        }
        if (key == GLFW_KEY_ESCAPE) {
            m_escPressed = true;
        }
        if (key == GLFW_KEY_SPACE) {
            m_spacePressed = true;
        }
        if (key == GLFW_KEY_ENTER) {
            m_enterPressed = true;
        }
    }
}
