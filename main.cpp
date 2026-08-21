#include <csignal>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
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
}
int main(void) {
    struct {
        glm::vec4 Position;
        glm::vec3 DisplayColour, AmbientColour, DiffuseColour, SpecularColour;
    } PointLightSource;
    struct {
        glm::vec3 AmbientReflectiveness, DiffuseReflectiveness, SpecularReflectiveness;
        float Shininess;
    } Material;
    std::puts("Enter the point light source's position in homogeneous coordinates (x y z w): ");
    std::scanf(" %f %f %f %f", &PointLightSource.Position.x, &PointLightSource.Position.y, &PointLightSource.Position.z, &PointLightSource.Position.w);
    bool const RenderLight = PointLightSource.Position.w != 0.0f;
    if(RenderLight) {
        std::puts("Enter the point light source's display colour (as a cube - yeah, I know, questionable choice) (r g b): ");
        std::scanf(" %f %f %f", &PointLightSource.DisplayColour.r, &PointLightSource.DisplayColour.g, &PointLightSource.DisplayColour.b);
    }
    std::puts("Enter the point light source's ambient lighting colour (r g b): ");
    std::scanf(" %f %f %f", &PointLightSource.AmbientColour.r, &PointLightSource.AmbientColour.g, &PointLightSource.AmbientColour.b);
    std::puts("Enter the point light source's diffuse lighting colour (r g b): ");
    std::scanf(" %f %f %f", &PointLightSource.DiffuseColour.r, &PointLightSource.DiffuseColour.g, &PointLightSource.DiffuseColour.b);
    std::puts("Enter the point light source's specular lighting colour (r g b): ");
    std::scanf(" %f %f %f", &PointLightSource.SpecularColour.r, &PointLightSource.SpecularColour.g, &PointLightSource.SpecularColour.b);
    std::puts("Enter the cube's material's ambient reflectiveness (r g b): ");
    std::scanf(" %f %f %f", &Material.AmbientReflectiveness.r, &Material.AmbientReflectiveness.g, &Material.AmbientReflectiveness.b);
    std::puts("Enter the cube's material's diffuse reflectiveness (r g b): ");
    std::scanf(" %f %f %f", &Material.DiffuseReflectiveness.r, &Material.DiffuseReflectiveness.g, &Material.DiffuseReflectiveness.b);
    std::puts("Enter the cube's material's specular reflectiveness (r g b): ");
    std::scanf(" %f %f %f", &Material.SpecularReflectiveness.r, &Material.SpecularReflectiveness.g, &Material.SpecularReflectiveness.b);
    std::puts("Enter the cube's material's shininess (single number): ");
    std::scanf(" %f", &Material.Shininess);
    std::putchar('\n');
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
    #endif
    glfwSetFramebufferSizeCallback(Window, HandleFramebufferSize);
    #ifndef NDEBUG_OPENGL
        std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    #endif
    glfwShowWindow(Window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    GLuint VAOs[1];
    glGenVertexArrays(1, VAOs);
    GLuint BufferObjects[2];
    glGenBuffers(2, BufferObjects);
    static GLfloat constexpr Vertices[] = {
        +5.0f, +5.0f, +5.0f,     0.0f,  0.0f, +1.0f,    1.0f, 1.0f,
        -5.0f, +5.0f, +5.0f,     0.0f,  0.0f, +1.0f,    0.0f, 1.0f,
        -5.0f, -5.0f, +5.0f,     0.0f,  0.0f, +1.0f,    0.0f, 0.0f,
        +5.0f, -5.0f, +5.0f,     0.0f,  0.0f, +1.0f,    1.0f, 0.0f,

        +5.0f, +5.0f, -5.0f,    +1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        +5.0f, +5.0f, +5.0f,    +1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        +5.0f, -5.0f, +5.0f,    +1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        +5.0f, -5.0f, -5.0f,    +1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

        -5.0f, +5.0f, -5.0f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
        +5.0f, +5.0f, -5.0f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
        +5.0f, -5.0f, -5.0f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
        -5.0f, -5.0f, -5.0f,     0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
        
        -5.0f, +5.0f, +5.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        -5.0f, +5.0f, -5.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -5.0f, -5.0f, -5.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        -5.0f, -5.0f, +5.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

        -5.0f, +5.0f, +5.0f,     0.0f, +1.0f,  0.0f,    1.0f, 1.0f,
        +5.0f, +5.0f, +5.0f,     0.0f, +1.0f,  0.0f,    0.0f, 1.0f,
        +5.0f, +5.0f, -5.0f,     0.0f, +1.0f,  0.0f,    0.0f, 0.0f,
        -5.0f, +5.0f, -5.0f,     0.0f, +1.0f,  0.0f,    1.0f, 0.0f,

        +5.0f, -5.0f, +5.0f,     0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
        -5.0f, -5.0f, +5.0f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
        -5.0f, -5.0f, -5.0f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
        +5.0f, -5.0f, -5.0f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void const *)(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void const *)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void const *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferObjects[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    GLuint const ShaderProgram1 = CreateLinkedShaderProgram(RESOURCE_DIR "VertexShader1.glsl", RESOURCE_DIR "FragmentShader1.glsl");
    GLuint ShaderProgram2;
    if(RenderLight)
        ShaderProgram2 = CreateLinkedShaderProgram(RESOURCE_DIR "VertexShader2.glsl", RESOURCE_DIR "FragmentShader2.glsl");
    //Note: The number suffix to each uniform refers to which shader program it is a part of.
    GLint const ModelMatUniform1 = GetUniformLocation(ShaderProgram1, "ModelMat");
    GLint const ModelCofactorMatUniform1 = GetUniformLocation(ShaderProgram1, "ModelCofactorMat");
    GLint const ViewProjMatUniform1 = GetUniformLocation(ShaderProgram1, "ViewProjMat");
    GLint const ViewInverseMatUniform1 = GetUniformLocation(ShaderProgram1, "ViewInverseMat");
    struct {
        GLint Position, AmbientColour, DiffuseColour, SpecularColour;
    } const PointLightSourceUniform1 {
        .Position = GetUniformLocation(ShaderProgram1, "PointLightSource.Position"),
        .AmbientColour = GetUniformLocation(ShaderProgram1, "PointLightSource.AmbientColour"),
        .DiffuseColour = GetUniformLocation(ShaderProgram1, "PointLightSource.DiffuseColour"),
        .SpecularColour = GetUniformLocation(ShaderProgram1, "PointLightSource.SpecularColour")
    };
    struct {
        GLint AmbientReflectiveness, DiffuseReflectiveness, SpecularReflectiveness, Shininess;
    } const MaterialUniform1 {
        .AmbientReflectiveness = GetUniformLocation(ShaderProgram1, "Material.AmbientReflectiveness"),
        .DiffuseReflectiveness = GetUniformLocation(ShaderProgram1, "Material.DiffuseReflectiveness"),
        .SpecularReflectiveness = GetUniformLocation(ShaderProgram1, "Material.SpecularReflectiveness"),
        .Shininess = GetUniformLocation(ShaderProgram1, "Material.Shininess")
    };
    GLint ModelViewProjMatUniform2;
    GLint ColourUniform2;
    if(RenderLight) {
        ModelViewProjMatUniform2 = GetUniformLocation(ShaderProgram2, "ModelViewProjMat");
        ColourUniform2 = GetUniformLocation(ShaderProgram2, "Colour");
    }
    glUseProgram(ShaderProgram1);
    static glm::mat4 constexpr ModelMat1 = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, 0.0f, -50.0f));
    glUniformMatrix4fv(ModelMatUniform1, 1, GL_FALSE, glm::value_ptr(ModelMat1));
    glm::mat4 const ModelAdjugateMat = glm::determinant(ModelMat1) * glm::inverse(ModelMat1);
    glUniformMatrix4fv(ModelCofactorMatUniform1, 1, GL_TRUE, glm::value_ptr(ModelAdjugateMat));
    glUniform4fv(PointLightSourceUniform1.Position, 1, glm::value_ptr(PointLightSource.Position));
    glUniform3fv(PointLightSourceUniform1.AmbientColour, 1, glm::value_ptr(PointLightSource.AmbientColour));
    glUniform3fv(PointLightSourceUniform1.DiffuseColour, 1, glm::value_ptr(PointLightSource.DiffuseColour));
    glUniform3fv(PointLightSourceUniform1.SpecularColour, 1, glm::value_ptr(PointLightSource.SpecularColour));
    glUniform3fv(MaterialUniform1.AmbientReflectiveness, 1, glm::value_ptr(Material.AmbientReflectiveness));
    glUniform3fv(MaterialUniform1.DiffuseReflectiveness, 1, glm::value_ptr(Material.DiffuseReflectiveness));
    glUniform3fv(MaterialUniform1.SpecularReflectiveness, 1, glm::value_ptr(Material.SpecularReflectiveness));
    glUniform1f(MaterialUniform1.Shininess, Material.Shininess);
    glm::mat4 ModelMat2;
    if(RenderLight) {
        glUseProgram(ShaderProgram2);
        glUniform3fv(ColourUniform2, 1, glm::value_ptr(PointLightSource.DisplayColour));
        ModelMat2 = glm::scale(glm::translate(glm_mat4_Identity, glm::vec3(PointLightSource.Position)), glm::vec3(0.1f));
    }
    static glm::mat4 constinit ViewAndProjMat[2] = { glm_mat4_Identity };
    glm::mat4 &ViewMat = ViewAndProjMat[0], &ProjMat = ViewAndProjMat[1];
    ProjMat = glm::perspective(glm::quarter_pi<float>(), WindowInitAspectRatio, FrustumNear, FrustumFar);//Would have constant initialised this too, but glm::perspective is not constexpr!
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if(glfwRawMouseMotionSupported())
        glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetWindowUserPointer(Window, ViewAndProjMat);
    glfwGetCursorPos(Window, &MouseXPos, &MouseYPos);
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
            ViewMat = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, 0.0f, +CameraLinearSpeed * DeltaTime)) * ViewMat;
        if(S && !W)
            ViewMat = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, 0.0f, -CameraLinearSpeed * DeltaTime)) * ViewMat;
        if(A && !D)
            ViewMat = glm::translate(glm_mat4_Identity, glm::vec3(+CameraLinearSpeed * DeltaTime, 0.0f, 0.0f)) * ViewMat;
        if(D && !A)
            ViewMat = glm::translate(glm_mat4_Identity, glm::vec3(-CameraLinearSpeed * DeltaTime, 0.0f, 0.0f)) * ViewMat;
        if(Space && !LeftShift)
            ViewMat = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, -CameraLinearSpeed * DeltaTime, 0.0f)) * ViewMat;
        if(LeftShift && !Space)
            ViewMat = glm::translate(glm_mat4_Identity, glm::vec3(0.0f, +CameraLinearSpeed * DeltaTime, 0.0f)) * ViewMat;
        glm::mat4 const ViewProjMat = ProjMat * ViewMat, ViewInverseMat = glm::inverse(ViewMat);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(RenderLight)
            glUseProgram(ShaderProgram1);
        glUniformMatrix4fv(ViewProjMatUniform1, 1, GL_FALSE, glm::value_ptr(ViewProjMat));
        glUniformMatrix4fv(ViewInverseMatUniform1, 1, GL_FALSE, glm::value_ptr(ViewInverseMat));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void const *)0);
        if(RenderLight) {
            glUseProgram(ShaderProgram2);
            glUniformMatrix4fv(ModelViewProjMatUniform2, 1, GL_FALSE, glm::value_ptr(ViewProjMat * ModelMat2));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void const *)0);
        }
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}