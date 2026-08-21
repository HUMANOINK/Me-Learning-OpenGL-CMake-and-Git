#include <csignal>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <stb/stb_image.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "GLFWCallbacks.h"
#include "OpenGLCallbacks.h"
#include "OGLShaderAbstraction.hpp"
namespace {
    EXTERN_C_BLOCK_START
        void AtQuickExit(void) {
            glfwTerminate();
            std::fflush(nullptr);
        }
        [[noreturn]] void Abort(int const) {
            std::quick_exit(EXIT_FAILURE);
        }
    EXTERN_C_BLOCK_END
    void SetupTexture2D(GLuint Texture, char const *ImageFilepath) {
        int TexWidth, TexHeight, TexChannels;
        glBindTexture(GL_TEXTURE_2D, Texture);
        unsigned char *TexImage = stbi_load(ImageFilepath, &TexWidth, &TexHeight, &TexChannels, 4);
        #ifndef NDEBUG_OPENGL
            if(!TexImage) {
                std::fprintf(stderr, "Failed to load texture!");
                std::exit(EXIT_FAILURE);
            }
        #endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TexWidth, TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexImage);
        stbi_image_free(TexImage);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
int main(void) {
    #ifndef NDEBUG_GLFW
        glfwSetErrorCallback(&HandleGLFWError);
        std::printf("GLFW version: %s\n", glfwGetVersionString());
    #endif
    if(!glfwInit()) [[unlikely]] {
        std::fputs("GLFW initialisation failed!", stderr);
        return EXIT_FAILURE;
    }
    std::atexit(&glfwTerminate);
    std::at_quick_exit(&AtQuickExit);
    std::signal(SIGABRT, &Abort);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #ifndef NDEBUG_OPENGL
        glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
    #endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *const Window = glfwCreateWindow(512, 512, "Hello, world!", nullptr, nullptr);
    glfwMakeContextCurrent(Window);
    glfwSwapInterval(1);
    if(!gladLoadGL(&glfwGetProcAddress)) [[unlikely]] {
        std::fputs("Glad failed to load OpenGL functions!", stderr);
        return EXIT_FAILURE;
    }
    #ifndef NDEBUG_OPENGL
        glDebugMessageCallback(&HandleOpenGLError, nullptr);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
    #endif
    glfwSetFramebufferSizeCallback(Window, HandleFramebufferSize);
    #ifndef NDEBUG_OPENGL
        std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    #endif
    glfwShowWindow(Window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint VAOs[2];
    glGenVertexArrays(2, VAOs);
    GLuint BufferObjects[5];
    glGenBuffers(5, BufferObjects);
    static GLfloat constexpr VertexPositions1[] = {
        +0.50f, +0.50f,
        0.00f, +0.75f,
        -0.50f, +0.50f,
        -0.75f,  0.00f,
        -0.50f, -0.50f,
        0.00f, -0.75f,
        +0.50f, -0.50f,
        +0.75f,  0.00f,
    };
    static GLubyte constexpr VertexColours[] = {
        111, 255, 174,
        27, 241, 144,
        39,  85,  16,
        40, 104, 229,
        111,  75, 219,
        93,  24, 227,
        53,   5, 147,
        156, 192, 252,
    };
    static GLubyte constexpr Indices1[] = {
        0, 1, 2,
        2, 3, 4,
        4, 5, 6,
        6, 7, 0,
    };
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferObjects[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositions1), VertexPositions1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void const *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, BufferObjects[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexColours), VertexColours, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void const *)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferObjects[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices1), Indices1, GL_STATIC_DRAW);
    static GLfloat constexpr VertexPositions2AndTexCoords[] = {
        +0.5f, +0.5f,   1.0f, 1.0f,
        -0.5f, +0.5f,   0.0f, 1.0f,
        -0.5f, -0.5f,   0.0f, 0.0f,
        +0.5f, -0.5f,   1.0f, 0.0f,
    };
    static GLubyte constexpr Indices2[] = {
        0, 1, 2,
        2, 3, 0,
    };
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferObjects[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositions2AndTexCoords), VertexPositions2AndTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void const *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void const *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferObjects[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);
    GLuint const ShaderProgram1 = CreateLinkedShaderProgram(RESOURCE_DIR "VertexShader1.glsl", RESOURCE_DIR "FragmentShader1.glsl");
    GLuint const VertexShader2 = CreateCompiledShader(GL_VERTEX_SHADER, RESOURCE_DIR "VertexShader2.glsl");
    GLuint const FragmentShader2 = CreateCompiledShader(GL_FRAGMENT_SHADER, RESOURCE_DIR "FragmentShader2.glsl");
    GLuint const FragmentShader3 = CreateCompiledShader(GL_FRAGMENT_SHADER, RESOURCE_DIR "FragmentShader3.glsl");
    GLuint const ShaderProgram2 = CreateLinkedShaderProgram(VertexShader2, FragmentShader2);
    GLuint const ShaderProgram3 = CreateLinkedShaderProgram(VertexShader2, FragmentShader3);
    glDeleteShader(VertexShader2);
    glDeleteShader(FragmentShader2);
    glDeleteShader(FragmentShader3);
    GLint const AlphaUniform = GetUniformLocation(ShaderProgram1, "Alpha");
    GLint const BlendUniform = GetUniformLocation(ShaderProgram2, "Blend");
    GLint const Texture1Uniform = GetUniformLocation(ShaderProgram2, "Texture1");
    GLint const Texture2Uniform = GetUniformLocation(ShaderProgram2, "Texture2");
    glUseProgram(ShaderProgram2);
    stbi_set_flip_vertically_on_load(true);
    GLuint Textures[2];
    glGenTextures(2, Textures);
    glActiveTexture(GL_TEXTURE0);
    SetupTexture2D(Textures[0], RESOURCE_DIR "container.jpg");
    glActiveTexture(GL_TEXTURE1);
    SetupTexture2D(Textures[1], RESOURCE_DIR "awesomeface.png");
    glUniform1i(Texture1Uniform, 0);
    glUniform1i(Texture2Uniform, 1);
    while(!glfwWindowShouldClose(Window)) {
        double Time = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAOs[0]);
        glUseProgram(ShaderProgram1);
        glUniform1f(AlphaUniform, static_cast<float>((std::cos(Time * 2.5) + 1.0) * 0.5));
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, (void const *)0);
        glBindVertexArray(VAOs[1]);
        glUseProgram(ShaderProgram3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void const *)0);
        glUseProgram(ShaderProgram2);
        glUniform1f(BlendUniform, static_cast<float>((std::cos(Time * 1.25) + 1.0) * 0.5));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void const *)0);
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}