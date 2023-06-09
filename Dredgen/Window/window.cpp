#include "window.h"

GLFWwindow *main_window;

HWND HWNDFromGLFWWindow(GLFWwindow *window) { return glfwGetWin32Window(window); }

Window::Window(uint32_t w, uint32_t h, const char *name) {
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow(w, h, name, NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        glfwTerminate();
        return;
    }
    m_window = main_window = window;
}

Window::~Window() { glfwTerminate(); }
