#ifndef NDEBUG_GLFW
    #include <cstdlib>
    #include <cstdio>
#endif
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include "CompileTimeConstants.hpp"
#include "GLFWCallbacks.hpp"
extern "C" {
    #ifndef NDEBUG_GLFW
    [[noreturn]] void HandleGLFWError(int const error_code, char const *const description) {
        std::fprintf(stderr, "[GLFW Error]\t\tCode: 0x%04X\tDecription: %s\n", error_code, description);
        std::exit(EXIT_FAILURE);
    }
    #endif
    void HandleFramebufferSize(GLFWwindow *const, int const width, int const height) {
        glViewport(0, 0, width, height);
    }
    void HandleCursorPosition(GLFWwindow* const window, double const xpos, double const ypos) {
        static double constexpr MouseSensitivity = 0.0004f;
        WindowUserPointerType &WindowUserPointerValue = *static_cast<WindowUserPointerType *>(glfwGetWindowUserPointer(window));
        glm::mat4 &ViewMat = WindowUserPointerValue.ViewMat;
        double const XChange = xpos - MouseXPos, YChange = ypos - MouseYPos;
        MouseXPos = xpos, MouseYPos = ypos;
        double const Magnitude = std::sqrt(XChange * XChange + YChange * YChange);
        ViewMat = glm::rotate(glm_mat4_Identity, static_cast<float>(MouseSensitivity * Magnitude), glm::vec3(static_cast<float>(YChange), static_cast<float>(XChange), 0.0f)) * ViewMat;
        WindowUserPointerValue.RecentMouseCallback = true;
    }
    void HandleScroll(GLFWwindow* const window, double const, double const yoffset) {
        static double constexpr ScrollSpeeed = -0.1;
        static double constinit TanVFOV = 1.0;
        WindowUserPointerType &WindowUserPointerValue = *static_cast<WindowUserPointerType *>(glfwGetWindowUserPointer(window));
        glm::mat4 &ProjMat = WindowUserPointerValue.ProjMat;
        TanVFOV *= std::exp(ScrollSpeeed * yoffset);
        ProjMat = glm::perspective(static_cast<float>(std::atan(TanVFOV)), WindowInitAspectRatio, FrustumNear, FrustumFar);
        WindowUserPointerValue.RecentMouseCallback = true;
    }
}