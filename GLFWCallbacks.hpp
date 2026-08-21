#include <GLFW/glfw3.h>
EXTERN_C_BLOCK_START
    #ifndef NDEBUG_GLFW
        [[noreturn]] void HandleGLFWError(int error_code, char const *description);
    #endif
    void HandleFramebufferSize(GLFWwindow *window, int width, int height);
    void HandleCursorPosition(GLFWwindow* window, double xpos, double ypos);
    void HandleScroll(GLFWwindow* window, double xoffset, double yoffset);
    inline double MouseXPos, MouseYPos;
EXTERN_C_BLOCK_END