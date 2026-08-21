#include <csignal>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <stb/stb_image.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "CompileTimeConstants.hpp"
#include "GLFWCallbacks.hpp"
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
    void SetupTexture2D(GLuint const Texture, char const *const ImageFilepath) {
        int TexWidth, TexHeight, TexChannels;
        glBindTexture(GL_TEXTURE_2D, Texture);
        static GLenum constexpr TexFormats[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
        static GLenum constexpr TexIntFormats[] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
        unsigned char *TexImage = stbi_load(ImageFilepath, &TexWidth, &TexHeight, &TexChannels, 0);
        #ifndef NDEBUG_OPENGL
        if(!TexImage) {
            std::fprintf(stderr, "Failed to load texture!");
            std::exit(EXIT_FAILURE);
        }
        #endif
        glTexImage2D(GL_TEXTURE_2D, 0, TexIntFormats[TexChannels - 1], TexWidth, TexHeight, 0, TexFormats[TexChannels - 1], GL_UNSIGNED_BYTE, TexImage);
        stbi_image_free(TexImage);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
int main(void) {
    #ifdef NDEBUG_GLFW
        glfwInit();
    #else
        glfwSetErrorCallback(&HandleGLFWError);
        std::printf("GLFW version: %s\n", glfwGetVersionString());
        if(!glfwInit()) [[unlikely]] {
            std::fputs("GLFW initialisation failed!", stderr);
            return EXIT_FAILURE;
        }
    #endif
    std::atexit(&glfwTerminate);
    std::at_quick_exit(&AtQuickExit);
    std::signal(SIGABRT, &Abort);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #ifndef NDEBUG_OPENGL
        glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
    #endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *const Window = glfwCreateWindow(WindowInitWidth, WindowInitHeight, "Hello, world!", nullptr, nullptr);
    #ifndef NDEBUG_GLFW
        if(!Window) [[unlikely]] {
            std::fputs("GLFW window creation failed, and in a way that wasn't reported to the error callback!", stderr);
            return EXIT_FAILURE;
        }
    #endif
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
        glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_FALSE);
    #endif
    glfwSetFramebufferSizeCallback(Window, HandleFramebufferSize);
    #ifndef NDEBUG_OPENGL
        std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    #endif
    glfwShowWindow(Window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    GLuint VAOs[1];
    glGenVertexArrays(1, VAOs);
    GLuint BufferObjects[2];
    glGenBuffers(2, BufferObjects);
    static GLfloat constexpr VertexPositionsAndTexCoords[] = {
        +5.0f, +5.0f, +5.0f,    1.0f, 1.0f,
        -5.0f, +5.0f, +5.0f,    0.0f, 1.0f,
        -5.0f, -5.0f, +5.0f,    0.0f, 0.0f,
        +5.0f, -5.0f, +5.0f,    1.0f, 0.0f,

        +5.0f, +5.0f, -5.0f,    1.0f, 1.0f,
        +5.0f, +5.0f, +5.0f,    0.0f, 1.0f,
        +5.0f, -5.0f, +5.0f,    0.0f, 0.0f,
        +5.0f, -5.0f, -5.0f,    1.0f, 0.0f,

        -5.0f, +5.0f, -5.0f,    1.0f, 1.0f,
        +5.0f, +5.0f, -5.0f,    0.0f, 1.0f,
        +5.0f, -5.0f, -5.0f,    0.0f, 0.0f,
        -5.0f, -5.0f, -5.0f,    1.0f, 0.0f,
        
        -5.0f, +5.0f, +5.0f,    1.0f, 1.0f,
        -5.0f, +5.0f, -5.0f,    0.0f, 1.0f,
        -5.0f, -5.0f, -5.0f,    0.0f, 0.0f,
        -5.0f, -5.0f, +5.0f,    1.0f, 0.0f,

        -5.0f, +5.0f, +5.0f,    1.0f, 1.0f,
        +5.0f, +5.0f, +5.0f,    0.0f, 1.0f,
        +5.0f, +5.0f, -5.0f,    0.0f, 0.0f,
        -5.0f, +5.0f, -5.0f,    1.0f, 0.0f,

        +5.0f, -5.0f, +5.0f,    1.0f, 1.0f,
        -5.0f, -5.0f, +5.0f,    0.0f, 1.0f,
        -5.0f, -5.0f, -5.0f,    0.0f, 0.0f,
        +5.0f, -5.0f, -5.0f,    1.0f, 0.0f,
    };
    static GLubyte constexpr Indices[] = {
         0,  1,  2,
         2,  3,  0,

         4,  5,  6,
         6,  7,  4,

         8,  9, 10,
        10, 11,  8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20,
    };
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferObjects[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionsAndTexCoords), VertexPositionsAndTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void const *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void const *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferObjects[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    GLuint const ShaderProgram = CreateLinkedShaderProgram(RESOURCE_DIR "VertexShader.glsl", RESOURCE_DIR "FragmentShader.glsl");
    GLint const TransformUniformLocation = GetUniformLocation(ShaderProgram, "Transform");
    GLint const Texture1Uniform = GetUniformLocation(ShaderProgram, "Texture1");
    GLint const Texture2Uniform = GetUniformLocation(ShaderProgram, "Texture2");
    glUseProgram(ShaderProgram);
    GLuint Textures[2];
    glGenTextures(2, Textures);
    stbi_set_flip_vertically_on_load(true);
    glActiveTexture(GL_TEXTURE0);
    SetupTexture2D(Textures[0], RESOURCE_DIR "container.jpg");
    glActiveTexture(GL_TEXTURE1);
    SetupTexture2D(Textures[1], RESOURCE_DIR "awesomeface.png");
    glUniform1i(Texture1Uniform, 0);
    glUniform1i(Texture2Uniform, 1);
    static glm::mat4 constinit ViewAndProjMat[2] = { glm_mat4_Identity };
    ViewAndProjMat[1] = glm::perspective(glm::quarter_pi<float>(), WindowInitAspectRatio, FrustumNear, FrustumFar);//Would have constant initialised this too, but glm::perspective is not constexpr!
    static glm::mat4 constexpr ModelMat = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, 0.0f, -50.0f));
    glfwGetCursorPos(Window, &MouseXPos, &MouseYPos);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if(glfwRawMouseMotionSupported())
        glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetWindowUserPointer(Window, &ViewAndProjMat);
    glfwSetCursorPosCallback(Window, HandleCursorPosition);
    glfwSetScrollCallback(Window, HandleScroll);
    double FrameStartTime, PrevFrameStartTime = 0.0;
    float DeltaTime;
    glfwSetTime(0.0);
    while(!glfwWindowShouldClose(Window)) {
        FrameStartTime = glfwGetTime();
        DeltaTime = static_cast<float>(FrameStartTime - PrevFrameStartTime);
        PrevFrameStartTime = FrameStartTime;
        static float constexpr CameraLinearSpeed = 20.0f;
        bool const W = glfwGetKey(Window, GLFW_KEY_W) != GLFW_RELEASE,
                   S = glfwGetKey(Window, GLFW_KEY_S) != GLFW_RELEASE,
                   A = glfwGetKey(Window, GLFW_KEY_A) != GLFW_RELEASE,
                   D = glfwGetKey(Window, GLFW_KEY_D) != GLFW_RELEASE,
                   Space = glfwGetKey(Window, GLFW_KEY_SPACE) != GLFW_RELEASE,
                   LeftShift = glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE;
        if(W && !S)
            ViewAndProjMat[0] = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, 0.0f, +CameraLinearSpeed * DeltaTime)) * ViewAndProjMat[0];
        if(S && !W)
            ViewAndProjMat[0] = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, 0.0f, -CameraLinearSpeed * DeltaTime)) * ViewAndProjMat[0];
        if(A && !D)
            ViewAndProjMat[0] = glm::translate(glm_mat4_Identity, glm::vec3(+CameraLinearSpeed * DeltaTime, 0.0f, 0.0f)) * ViewAndProjMat[0];
        if(D && !A)
            ViewAndProjMat[0] = glm::translate(glm_mat4_Identity, glm::vec3(-CameraLinearSpeed * DeltaTime, 0.0f, 0.0f)) * ViewAndProjMat[0];
        if(Space && !LeftShift)
            ViewAndProjMat[0] = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, -CameraLinearSpeed * DeltaTime, 0.0f)) * ViewAndProjMat[0];
        if(LeftShift && !Space)
            ViewAndProjMat[0] = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, +CameraLinearSpeed * DeltaTime, 0.0f)) * ViewAndProjMat[0];/*
        if(glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            ViewAndProjMat[0] = glm::rotate(glm_mat4_Identity, CameraAngularSpeed * DeltaTime, glm::vec3(0.0f, +1.0f, 0.0f)) * ViewAndProjMat[0];
        if(glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS)
            ViewAndProjMat[0] = glm::rotate(glm_mat4_Identity, CameraAngularSpeed * DeltaTime, glm::vec3(0.0f, -1.0f, 0.0f)) * ViewAndProjMat[0];
        if(glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
            ViewAndProjMat[0] = glm::rotate(glm_mat4_Identity, CameraAngularSpeed * DeltaTime, glm::vec3(-1.0f, 0.0f, 0.0f)) * ViewAndProjMat[0];
        if(glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS)
            ViewAndProjMat[0] = glm::rotate(glm_mat4_Identity, CameraAngularSpeed * DeltaTime, glm::vec3(+1.0f, 0.0f, 0.0f)) * ViewAndProjMat[0];*/
        glm::mat4 const ViewProjMat = ViewAndProjMat[1] * ViewAndProjMat[0];
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniformMatrix4fv(TransformUniformLocation, 1, GL_FALSE, glm::value_ptr(ViewProjMat * ModelMat));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void const *)0);
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}