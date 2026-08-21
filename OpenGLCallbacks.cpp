#ifndef NDEBUG_OPENGL
    #include <cstdlib>
    #include <cstdio>
    #include <utility>
    #include "OpenGLCallbacks.h"
    EXTERN_C_BLOCK_START
        namespace {
            inline char const *OGLErrorSource(GLenum const source) {
                switch(source) {
                case GL_DEBUG_SOURCE_API:
                    return "API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                    return "Window system";
                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                    return "Shader compiler";
                case GL_DEBUG_SOURCE_THIRD_PARTY:
                    return "Third party";
                case GL_DEBUG_SOURCE_APPLICATION:
                    return "Application";
                case GL_DEBUG_SOURCE_OTHER:
                    return "Other";
                default:
                    std::unreachable();
                }
            }
            inline char const *OGLErrorType(GLenum const type) {
                switch(type) {
                case GL_DEBUG_TYPE_ERROR:
                    return "Error";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    return "Deprecated behaviour";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    return "Undefined behaviour";
                case GL_DEBUG_TYPE_PORTABILITY:
                    return "Portability";
                case GL_DEBUG_TYPE_PERFORMANCE:
                    return "Performance";
                case GL_DEBUG_TYPE_MARKER:
                    return "Marker";
                case GL_DEBUG_TYPE_PUSH_GROUP:
                    return "Push group";
                case GL_DEBUG_TYPE_POP_GROUP:
                    return "Pop group";
                case GL_DEBUG_TYPE_OTHER:  
                    return "Other";
                default:
                    std::unreachable();
                }
            }
            inline char const *OGLErrorSeverity(GLenum const severity) {
                switch(severity) {
                case GL_DEBUG_SEVERITY_HIGH:
                    return "High";
                case GL_DEBUG_SEVERITY_MEDIUM:
                    return "Medium";
                case GL_DEBUG_SEVERITY_LOW:
                    return "Low";
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    return "Notification";
                default:
                    std::unreachable();
                }
            }
        }
        void GLAD_API_PTR HandleOpenGLError(
            [[maybe_unused]] GLenum const source,
            [[maybe_unused]] GLenum const type,
            [[maybe_unused]] GLuint const id,
            [[maybe_unused]] GLenum const severity,
            [[maybe_unused]] GLsizei const length,
            [[maybe_unused]] GLchar const *const message,
            [[maybe_unused]] void const *const userParam
        ) {
            std::fprintf(stderr, "[OpenGL Error]\t\tSource: %s\t\tType: %s\t\tID: 0x%04X\t\tSeverity: %s\nMessage: %s\n",
                        OGLErrorSource(source), OGLErrorType(type), id, OGLErrorSeverity(severity), message);
            if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
                std::exit(EXIT_FAILURE);
        }
    EXTERN_C_BLOCK_END
#endif