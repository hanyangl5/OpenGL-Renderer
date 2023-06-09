#pragma once
#include <cstdint>
#include <stdint.h>

#include <GLFW/glfw3.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

class Window {
  public:
    Window(uint32_t w, uint32_t h, const char *name);
    ~Window();
    GLFWwindow *m_window;
};