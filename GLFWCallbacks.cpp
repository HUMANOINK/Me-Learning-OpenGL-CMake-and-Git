#ifndef NDEBUG_GLFW
    #include <cstdlib>
    #include <cstdio>
#endif
#include <glad/gl.h>
#include "GLFWCallbacks.h"
extern "C" {
    #ifndef NDEBUG_GLFW
        [[noreturn]] void HandleGLFWError(int const error_code, char const *const description) {
            std::fprintf(stderr, "[GLFW Error]\t\tCode: 0x%04X\tDecription: %s\n", error_code, description);
            std::exit(EXIT_FAILURE);
        }
    #endif
    void HandleFramebufferSize([[maybe_unused]] GLFWwindow *const window, int const width, int const height) {
        glViewport(0, 0, width, height);
    }
}