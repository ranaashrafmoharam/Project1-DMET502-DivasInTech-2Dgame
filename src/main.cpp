#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/Constants.h"
#include "core/Input.h"
#include "core/Game.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
#ifdef _WIN32
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) > 0) {
        std::string path(buffer);
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            SetCurrentDirectoryA(path.substr(0, pos).c_str());
        }
    }
#endif

    bool autoTest = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--auto-test") {
            autoTest = true;
        }
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        static_cast<int>(Constants::WINDOW_WIDTH),
        static_cast<int>(Constants::WINDOW_HEIGHT),
        "Ninjago: Defend the Monastery (DMET 502)",
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwShowWindow(window);
    glfwFocusWindow(window);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int fbw = 0, fbh = 0;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Input::instance().init(window);

    Game game;
    if (autoTest) {
        game.enableAutoTest(true);
    }
    if (!game.init(window)) {
        std::cerr << "Failed to initialize Game" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    game.run();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
