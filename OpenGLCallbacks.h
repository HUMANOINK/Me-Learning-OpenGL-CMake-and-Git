#include <glad/gl.h>
#ifndef NDEBUG_OPENGL
    EXTERN_C void GLAD_API_PTR HandleOpenGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam);
#endif