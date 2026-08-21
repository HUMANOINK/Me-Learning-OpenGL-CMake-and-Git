#ifndef NDEBUG_GLFW
    #include <cstdlib>
    #include <cstdio>
#endif
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include "CompileTimeConstants.hpp"
#include "GLFWCallbacks.hpp"
EXTERN_C_BLOCK_START
    #ifndef NDEBUG_GLFW
        [[noreturn]] void HandleGLFWError([[maybe_unused]] int const error_code, [[maybe_unused]] char const *const description) {
            std::fprintf(stderr, "[GLFW Error]\t\tCode: 0x%04X\tDecription: %s\n", error_code, description);
            std::exit(EXIT_FAILURE);
        }
    #endif
    void HandleFramebufferSize(GLFWwindow *const, int const width, int const height) {
        glViewport(0, 0, width, height);
    }
    void HandleCursorPosition(GLFWwindow *const window, double xpos, double ypos) {
        static double constexpr MouseSensitivity = 0.0004f;
        glm::mat4 &ViewMat = (*static_cast<glm::mat4 (*)[2]>(glfwGetWindowUserPointer(window)))[0];
        double const XChange = xpos - MouseXPos, YChange = ypos - MouseYPos;
        MouseXPos = xpos, MouseYPos = ypos;
        double const Magnitude = std::sqrt(XChange * XChange + YChange * YChange);
        ViewMat = glm::rotate(glm_mat4_Identity, static_cast<float>(MouseSensitivity * Magnitude), glm::vec3(static_cast<float>(YChange), static_cast<float>(XChange), 0.0f)) * ViewMat;
    }
    void HandleScroll(GLFWwindow *const window, double const, double const yoffset) {
        static double constexpr ScrollSpeeed = -0.1;
        static double constinit TanVFOV = 1.0;
        glm::mat4 &ProjMat = (*static_cast<glm::mat4 (*)[2]>(glfwGetWindowUserPointer(window)))[1];
        TanVFOV *= std::exp(ScrollSpeeed * yoffset);
        ProjMat = glm::perspective(static_cast<float>(std::atan(TanVFOV)), WindowInitAspectRatio, FrustumNear, FrustumFar);
    }
EXTERN_C_BLOCK_END