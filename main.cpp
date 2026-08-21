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
#define CUBES true
#define POINT_LIGHT_SOURCES true
#define DIRECTIONAL_LIGHT_SOURCES false
#define SPOTLIGHT_LIGHT_SOURCES false
#define FLASHLIGHT_LIGHT_SOURCES false
inline GLuint constexpr N_CUBES = 3;
inline GLuint constexpr N_POINT_LIGHT_SOURCES = 3;
inline GLuint constexpr N_DIRECTIONAL_LIGHT_SOURCES = 0;
inline GLuint constexpr N_SPOTLIGHT_LIGHT_SOURCES = 0;
inline GLuint constexpr N_FLASHLIGHT_LIGHT_SOURCES = 0;
#if CUBES
    static_assert(N_CUBES > 0u, "Define CUBES to be false if you want there to be 0 cubes.");
#endif
#if POINT_LIGHT_SOURCES
    static_assert(N_POINT_LIGHT_SOURCES > 0u, "Define POINT_LIGHT_SOURCES to be false if you want there to be 0 point light sources.");
#endif
#if DIRECTIONAL_LIGHT_SOURCES
    static_assert(N_DIRECTIONAL_LIGHT_SOURCES > 0u, "Define DIRECTIONAL_LIGHT_SOURCES to be false if you want there to be 0 directional light sources.");
#endif
#if SPOTLIGHT_LIGHT_SOURCES
    static_assert(N_SPOTLIGHT_LIGHT_SOURCES > 0u, "Define SPOTLIGHT_LIGHT_SOURCES to be false if you want there to be 0 spotlight light sources.");
#endif
#if FLASHLIGHT_LIGHT_SOURCES
    static_assert(N_FLASHLIGHT_LIGHT_SOURCES > 0u, "Define FLASHLIGHT_LIGHT_SOURCES to be false if you want there to be 0 flashlight light sources.");
#endif
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
        static GLenum constexpr TexIntFormats[] = { GL_R8, GL_RG8, GL_SRGB8, GL_SRGB8_ALPHA8 };
        unsigned char *const TexImage = stbi_load(ImageFilepath, &TexWidth, &TexHeight, &TexChannels, 0);
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
    #if CUBES
        struct {
            glm::vec3 Position;
            struct {
                float Shininess;
            } Material;
        } Cubes[N_CUBES];
    #endif
    #if POINT_LIGHT_SOURCES
        struct {
            glm::vec3 Position;
            glm::vec3 DisplayColour, AmbientColour, DiffuseColour, SpecularColour;
            float LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
            float ModelSpacePointSize, Falloff;
            int VolumetricSize;
        } PointLightSources[N_POINT_LIGHT_SOURCES];
    #endif
    #if DIRECTIONAL_LIGHT_SOURCES
        struct {
            glm::vec3 Direction;
            glm::vec3 AmbientColour, DiffuseColour, SpecularColour;
        } DirectionalLightSources[N_DIRECTIONAL_LIGHT_SOURCES];
    #endif
    #if SPOTLIGHT_LIGHT_SOURCES
        struct {
            glm::vec3 Position, Direction;
            glm::vec3 DisplayColour, AmbientColour, DiffuseColour, SpecularColour;
            float InnerCutoffAngleCosine, OuterCutoffAngleCosine;
            float LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
            float ModelSpacePointSize, Falloff;
            int VolumetricSize;
        } SpotlightLightSources[N_SPOTLIGHT_LIGHT_SOURCES];
    #endif
    #if FLASHLIGHT_LIGHT_SOURCES
        struct {
            glm::vec3 AmbientColour, DiffuseColour, SpecularColour;
            float InnerCutoffAngleCosine, OuterCutoffAngleCosine;
            float LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
        } FlashlightLightSources[N_FLASHLIGHT_LIGHT_SOURCES];
    #endif
    #if CUBES
        for(GLuint i = 0; i < N_CUBES; ++i) {
            std::printf("Enter the position of cube %u (x y z):\n", i);
            std::scanf("%f%f%f", &Cubes[i].Position.x, &Cubes[i].Position.y, &Cubes[i].Position.z);
            std::printf("Enter the shininess of cube %u:\n", i);
            std::scanf("%f", &Cubes[i].Material.Shininess);
        }
    #endif
    #if POINT_LIGHT_SOURCES
        for(GLuint i = 0; i < N_POINT_LIGHT_SOURCES; ++i) {
            std::printf("Enter the position of point light source %u (x y z):\n", i);
            std::scanf("%f%f%f", &PointLightSources[i].Position.x, &PointLightSources[i].Position.y, &PointLightSources[i].Position.z);
            std::printf("Enter the display colour of point light source %u (r g b):\n", i);
            std::scanf("%f%f%f", &PointLightSources[i].DisplayColour.r, &PointLightSources[i].DisplayColour.g, &PointLightSources[i].DisplayColour.b);
            std::printf("Enter the ambient, diffuse and specular colours of point light source %u (r g b):\n", i);
            std::scanf("%f%f%f%f%f%f%f%f%f",
                &PointLightSources[i].AmbientColour.r, &PointLightSources[i].AmbientColour.g, &PointLightSources[i].AmbientColour.b,
                &PointLightSources[i].DiffuseColour.r, &PointLightSources[i].DiffuseColour.g, &PointLightSources[i].DiffuseColour.b,
                &PointLightSources[i].SpecularColour.r, &PointLightSources[i].SpecularColour.g, &PointLightSources[i].SpecularColour.b);
            std::printf("Enter the linear and quadratic attenuation coefficients for point light source %u:\n", i);
            std::scanf("%f%f", &PointLightSources[i].LinearAttenuationCoefficient, &PointLightSources[i].QuadraticAttenuationCoefficient);
            std::printf("Enter the model-space point size of point light source %u:\n", i);
            std::scanf("%f", &PointLightSources[i].ModelSpacePointSize);
            std::printf("Enter the display falloff factor of point light source %u:\n", i);
            std::scanf("%f", &PointLightSources[i].Falloff);
            std::printf("Enter, as an integer, whether to use volumetric size (true) or its alternative (false) for size factor calculation for point light source %u:\n", i);
            std::scanf("%i", &PointLightSources[i].VolumetricSize);
        }
    #endif
    #if DIRECTIONAL_LIGHT_SOURCES
        for(GLuint i = 0; i < N_DIRECTIONAL_LIGHT_SOURCES; ++i) {
            std::printf("Enter the direction of directional light source %u (x y z):\n", i);
            std::scanf("%f%f%f", &DirectionalLightSources[i].Direction.x, &DirectionalLightSources[i].Direction.y, &DirectionalLightSources[i].Direction.z);
            std::printf("Enter the ambient, diffuse and specular colours of directional light source %u (r g b):\n", i);
            std::scanf("%f%f%f%f%f%f%f%f%f",
                &DirectionalLightSources[i].AmbientColour.r, &DirectionalLightSources[i].AmbientColour.g, &DirectionalLightSources[i].AmbientColour.b,
                &DirectionalLightSources[i].DiffuseColour.r, &DirectionalLightSources[i].DiffuseColour.g, &DirectionalLightSources[i].DiffuseColour.b,
                &DirectionalLightSources[i].SpecularColour.r, &DirectionalLightSources[i].SpecularColour.g, &DirectionalLightSources[i].SpecularColour.b);
        }
    #endif
    #if SPOTLIGHT_LIGHT_SOURCES
        for(GLuint i = 0; i < N_SPOTLIGHT_LIGHT_SOURCES; ++i) {
            std::printf("Enter the position of spotlight light source %u (x y z):\n", i);
            std::scanf("%f%f%f", &SpotlightLightSources[i].Position.x, &SpotlightLightSources[i].Position.y, &SpotlightLightSources[i].Position.z);
            std::printf("Enter the direction of spotlight light source %u (x y z):\n", i);
            std::scanf("%f%f%f", &SpotlightLightSources[i].Direction.x, &SpotlightLightSources[i].Direction.y, &SpotlightLightSources[i].Direction.z);
            std::printf("Enter the display colour of spotlight light source %u (r g b):\n", i);
            std::scanf("%f%f%f", &SpotlightLightSources[i].DisplayColour.r, &SpotlightLightSources[i].DisplayColour.g, &SpotlightLightSources[i].DisplayColour.b);
            std::printf("Enter the ambient, diffuse and specular colours of spotlight light source %u (r g b):\n", i);
            std::scanf("%f%f%f%f%f%f%f%f%f",
                &SpotlightLightSources[i].AmbientColour.r, &SpotlightLightSources[i].AmbientColour.g, &SpotlightLightSources[i].AmbientColour.b,
                &SpotlightLightSources[i].DiffuseColour.r, &SpotlightLightSources[i].DiffuseColour.g, &SpotlightLightSources[i].DiffuseColour.b,
                &SpotlightLightSources[i].SpecularColour.r, &SpotlightLightSources[i].SpecularColour.g, &SpotlightLightSources[i].SpecularColour.b);
            std::printf("Enter the cosines of the inner and outer cutoff angles for spotlight light source %u:\n", i);
            std::scanf("%f%f", &SpotlightLightSources[i].InnerCutoffAngleCosine, &SpotlightLightSources[i].OuterCutoffAngleCosine);
            std::printf("Enter the linear and quadratic attenuation coefficients for spotlight light source %u:\n", i);
            std::scanf("%f%f", &SpotlightLightSources[i].LinearAttenuationCoefficient, &SpotlightLightSources[i].QuadraticAttenuationCoefficient);
            std::printf("Enter the model-space point size of spotlight light source %u:\n", i);
            std::scanf("%f", &SpotlightLightSources[i].ModelSpacePointSize);
            std::printf("Enter the display falloff factor of point light source %u:\n", i);
            std::scanf("%f", &SpotlightLightSources[i].Falloff);
            std::printf("Enter, as an integer, whether to use volumetric size (true) or its alternative (false) for size factor calculation for spotlight light source %u:\n", i);
            std::scanf("%i", &SpotlightLightSources[i].VolumetricSize);
        }
    #endif
    #if FLASHLIGHT_LIGHT_SOURCES
        for(GLuint i = 0; i < N_FLASHLIGHT_LIGHT_SOURCES; ++i) {
            std::printf("Enter the ambient, diffuse and specular colours of flashlight light source %u (r g b):\n", i);
            std::scanf("%f%f%f%f%f%f%f%f%f",
                &FlashlightLightSources[i].AmbientColour.r, &FlashlightLightSources[i].AmbientColour.g, &FlashlightLightSources[i].AmbientColour.b,
                &FlashlightLightSources[i].DiffuseColour.r, &FlashlightLightSources[i].DiffuseColour.g, &FlashlightLightSources[i].DiffuseColour.b,
                &FlashlightLightSources[i].SpecularColour.r, &FlashlightLightSources[i].SpecularColour.g, &FlashlightLightSources[i].SpecularColour.b);
            std::printf("Enter the cosines of the inner and outer cutoff angles for flashlight light source %u:\n", i);
            std::scanf("%f%f", &FlashlightLightSources[i].InnerCutoffAngleCosine, &FlashlightLightSources[i].OuterCutoffAngleCosine);
            std::printf("Enter the linear and quadratic attenuation coefficients for flashlight light source %u:\n", i);
            std::scanf("%f%f", &FlashlightLightSources[i].LinearAttenuationCoefficient, &FlashlightLightSources[i].QuadraticAttenuationCoefficient);
        }
        #endif
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
    if(!gladLoadGL(glfwGetProcAddress)) [[unlikely]] {
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
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    #if CUBES
        GLuint BufferObjects[2u];
        glGenBuffers(2, BufferObjects);
        static GLfloat constexpr Vertices[] = {
            +1, +1, +1,     0,  0, +1,    1, 1,
            -1, +1, +1,     0,  0, +1,    0, 1,
            -1, -1, +1,     0,  0, +1,    0, 0,
            +1, -1, +1,     0,  0, +1,    1, 0,

            +1, +1, -1,    +1,  0,  0,    1, 1,
            +1, +1, +1,    +1,  0,  0,    0, 1,
            +1, -1, +1,    +1,  0,  0,    0, 0,
            +1, -1, -1,    +1,  0,  0,    1, 0,

            -1, +1, -1,     0,  0, -1,    1, 1,
            +1, +1, -1,     0,  0, -1,    0, 1,
            +1, -1, -1,     0,  0, -1,    0, 0,
            -1, -1, -1,     0,  0, -1,    1, 0,
            
            -1, +1, +1,    -1,  0,  0,    1, 1,
            -1, +1, -1,    -1,  0,  0,    0, 1,
            -1, -1, -1,    -1,  0,  0,    0, 0,
            -1, -1, +1,    -1,  0,  0,    1, 0,

            -1, +1, +1,     0, +1,  0,    1, 1,
            +1, +1, +1,     0, +1,  0,    0, 1,
            +1, +1, -1,     0, +1,  0,    0, 0,
            -1, +1, -1,     0, +1,  0,    1, 0,

            +1, -1, +1,     0, -1,  0,    1, 1,
            -1, -1, +1,     0, -1,  0,    0, 1,
            -1, -1, -1,     0, -1,  0,    0, 0,
            +1, -1, -1,     0, -1,  0,    1, 0,
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
    #endif
    glBindVertexArray(VAO);
    #if CUBES
        glBindBuffer(GL_ARRAY_BUFFER, BufferObjects[0u]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 8u * sizeof(GLfloat), (void const *)(0u * sizeof(GLfloat)));
        glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, 8u * sizeof(GLfloat), (void const *)(3u * sizeof(GLfloat)));
        glVertexAttribPointer(2u, 2, GL_FLOAT, GL_FALSE, 8u * sizeof(GLfloat), (void const *)(6u * sizeof(GLfloat)));
        glEnableVertexAttribArray(0u);
        glEnableVertexAttribArray(1u);
        glEnableVertexAttribArray(2u);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferObjects[1u]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
        GLuint const ShaderProgram1 = CreateLinkedShaderProgram(RESOURCE_DIR "VertexShader1.vert", RESOURCE_DIR "FragmentShader1.frag");
    #endif
    #if POINT_LIGHT_SOURCES || N_SPOTLIGHT_LIGHT_SOURCES
        GLuint const ShaderProgram2 = CreateLinkedShaderProgram(RESOURCE_DIR "VertexShader2.vert", RESOURCE_DIR "FragmentShader2.frag");
    #endif
    //Note: The number suffix to each uniform refers to which shader program it is a part of.
    #if CUBES
        GLint const ModelMatUniform1 = GetUniformLocation(ShaderProgram1, "ModelMat");
        GLint const ModelAdjugateMatUniform1 = GetUniformLocation(ShaderProgram1, "ModelAdjugateMat");
        GLint const ViewProjMatUniform1 = GetUniformLocation(ShaderProgram1, "ViewProjMat");
        GLint const ViewInverseMatUniform1 = GetUniformLocation(ShaderProgram1, "ViewInverseMat");
        #if POINT_LIGHT_SOURCES
            struct {
                GLint Position;
                GLint AmbientColour, DiffuseColour, SpecularColour;
                GLint LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
            } const PointLightSourcesUniform1[] {
                {
                    .Position = GetUniformLocation(ShaderProgram1, "PointLightSources[0].Position"),
                    .AmbientColour = GetUniformLocation(ShaderProgram1, "PointLightSources[0].AmbientColour"),
                    .DiffuseColour = GetUniformLocation(ShaderProgram1, "PointLightSources[0].DiffuseColour"),
                    .SpecularColour = GetUniformLocation(ShaderProgram1, "PointLightSources[0].SpecularColour"),
                    .LinearAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "PointLightSources[0].LinearAttenuationCoefficient"),
                    .QuadraticAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "PointLightSources[0].QuadraticAttenuationCoefficient"),
                },
                {
                    .Position = GetUniformLocation(ShaderProgram1, "PointLightSources[1].Position"),
                    .AmbientColour = GetUniformLocation(ShaderProgram1, "PointLightSources[1].AmbientColour"),
                    .DiffuseColour = GetUniformLocation(ShaderProgram1, "PointLightSources[1].DiffuseColour"),
                    .SpecularColour = GetUniformLocation(ShaderProgram1, "PointLightSources[1].SpecularColour"),
                    .LinearAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "PointLightSources[1].LinearAttenuationCoefficient"),
                    .QuadraticAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "PointLightSources[1].QuadraticAttenuationCoefficient"),
                },
                {
                    .Position = GetUniformLocation(ShaderProgram1, "PointLightSources[2].Position"),
                    .AmbientColour = GetUniformLocation(ShaderProgram1, "PointLightSources[2].AmbientColour"),
                    .DiffuseColour = GetUniformLocation(ShaderProgram1, "PointLightSources[2].DiffuseColour"),
                    .SpecularColour = GetUniformLocation(ShaderProgram1, "PointLightSources[2].SpecularColour"),
                    .LinearAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "PointLightSources[2].LinearAttenuationCoefficient"),
                    .QuadraticAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "PointLightSources[2].QuadraticAttenuationCoefficient"),
                },
            };
        #endif
        #if DIRECTIONAL_LIGHT_SOURCES
            struct {
                GLint Direction;
                GLint AmbientColour, DiffuseColour, SpecularColour;
            } const DirectionalLightSourcesUniform1[] {
                {
                    .Direction = GetUniformLocation(ShaderProgram1, "DirectionalLightSources[0].Direction"),
                    .AmbientColour = GetUniformLocation(ShaderProgram1, "DirectionalLightSources[0].AmbientColour"),
                    .DiffuseColour = GetUniformLocation(ShaderProgram1, "DirectionalLightSources[0].DiffuseColour"),
                    .SpecularColour = GetUniformLocation(ShaderProgram1, "DirectionalLightSources[0].SpecularColour"),
                },
            };
        #endif
        #if SPOTLIGHT_LIGHT_SOURCES
            struct {
                GLint Position, Direction;
                GLint AmbientColour, DiffuseColour, SpecularColour;
                GLint InnerCutoffAngleCosine, OuterCutoffAngleCosine;
                GLint LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
            } const SpotlightLightSourcesUniform1[] {
                {
                    .Position = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].Position"),
                    .Direction = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].Direction"),
                    .AmbientColour = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].AmbientColour"),
                    .DiffuseColour = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].DiffuseColour"),
                    .SpecularColour = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].SpecularColour"),
                    .InnerCutoffAngleCosine = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].InnerCutoffAngleCosine"),
                    .OuterCutoffAngleCosine = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].OuterCutoffAngleCosine"),
                    .LinearAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].LinearAttenuationCoefficient"),
                    .QuadraticAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "SpotlightLightSources[0].QuadraticAttenuationCoefficient"),
                },
            };
        #endif
        #if FLASHLIGHT_LIGHT_SOURCES
            struct {
                GLint AmbientColour, DiffuseColour, SpecularColour;
                GLint InnerCutoffAngleCosine, OuterCutoffAngleCosine;
                GLint LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
            } const FlashlightLightSourcesUniform1[] {
                {
                    .AmbientColour = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].AmbientColour"),
                    .DiffuseColour = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].DiffuseColour"),
                    .SpecularColour = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].SpecularColour"),
                    .InnerCutoffAngleCosine = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].InnerCutoffAngleCosine"),
                    .OuterCutoffAngleCosine = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].OuterCutoffAngleCosine"),
                    .LinearAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].LinearAttenuationCoefficient"),
                    .QuadraticAttenuationCoefficient = GetUniformLocation(ShaderProgram1, "FlashlightLightSources[0].QuadraticAttenuationCoefficient"),
                },
            };
        #endif
        struct {
            GLint AmbientReflectivenessMap, DiffuseReflectivenessMap, SpecularReflectivenessMap;
            GLint Shininess;
        } const MaterialUniform1 {
            .AmbientReflectivenessMap = GetUniformLocation(ShaderProgram1, "Material.AmbientReflectivenessMap"),
            .DiffuseReflectivenessMap = GetUniformLocation(ShaderProgram1, "Material.DiffuseReflectivenessMap"),
            .SpecularReflectivenessMap = GetUniformLocation(ShaderProgram1, "Material.SpecularReflectivenessMap"),
            .Shininess = GetUniformLocation(ShaderProgram1, "Material.Shininess"),
        };
    #endif
    #if POINT_LIGHT_SOURCES || N_SPOTLIGHT_LIGHT_SOURCES
        GLint const ModelViewProjMatUniform2 = GetUniformLocation(ShaderProgram2, "ModelViewProjMat");
        GLint const ModelSpacePointSizeUniform2 = GetUniformLocation(ShaderProgram2, "ModelSpacePointSize");
        GLint const VolumetrixSizeUniform2 = GetUniformLocation(ShaderProgram2, "VolumetricSize");
        GLint const ColourUniform2 = GetUniformLocation(ShaderProgram2, "Colour");
        GLint const FalloffUniform2 = GetUniformLocation(ShaderProgram2, "Falloff");
    #endif
    #if CUBES
        glm::mat4 const CubeModelMats[] = {
            glm::scale(glm::translate(glm_mat4_Identity, Cubes[0u].Position), glm::vec3(5.0f)),
            glm::scale(glm::translate(glm_mat4_Identity, Cubes[1u].Position), glm::vec3(5.0f)),
            glm::scale(glm::translate(glm_mat4_Identity, Cubes[2u].Position), glm::vec3(5.0f)),
        };
        glm::mat4 const CubeModelAdjugateMats[] = {
            glm::determinant(CubeModelMats[0u]) * glm::inverse(CubeModelMats[0u]),
            glm::determinant(CubeModelMats[1u]) * glm::inverse(CubeModelMats[1u]),
            glm::determinant(CubeModelMats[2u]) * glm::inverse(CubeModelMats[2u]),
        };
    #endif
    #if POINT_LIGHT_SOURCES
        glm::mat4 const PointLightSourcesModelMats[] = {
            glm::translate(glm_mat4_Identity, glm::vec3(PointLightSources[0u].Position)),
            glm::translate(glm_mat4_Identity, glm::vec3(PointLightSources[1u].Position)),
            glm::translate(glm_mat4_Identity, glm::vec3(PointLightSources[2u].Position)),
        };
    #endif
    #if SPOTLIGHT_LIGHT_SOURCES
        glm::mat4 const SpotlightLightSourcesModelMats[] = {
            glm::translate(glm_mat4_Identity, glm::vec3(SpotlightLightSources[0u].Position)),
        };
    #endif
    static WindowUserPointerType constinit WindowUserPointerValue =  {
        .ViewMat{ 1.0f },
        .RecentMouseCallback = false,
    };
    glm::mat4 &ViewMat = WindowUserPointerValue.ViewMat, &ProjMat = WindowUserPointerValue.ProjMat;
    ProjMat = glm::perspective(glm::quarter_pi<float>(), WindowInitAspectRatio, FrustumNear, FrustumFar);//Would have constant initialised this too, but glm::perspective is not constexpr!
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if(glfwRawMouseMotionSupported())
        glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetWindowUserPointer(Window, &WindowUserPointerValue);
    glfwGetCursorPos(Window, &MouseXPos, &MouseYPos);
    glfwSetCursorPosCallback(Window, HandleCursorPosition);
    glfwSetScrollCallback(Window, HandleScroll);
    #if CUBES
        glUseProgram(ShaderProgram1);
        #if POINT_LIGHT_SOURCES
            for(GLuint i = 0; i < N_POINT_LIGHT_SOURCES; ++i) {
                glUniform3fv(PointLightSourcesUniform1[i].Position, 1, glm::value_ptr(PointLightSources[i].Position));
                glUniform3fv(PointLightSourcesUniform1[i].AmbientColour, 1, glm::value_ptr(PointLightSources[i].AmbientColour));
                glUniform3fv(PointLightSourcesUniform1[i].DiffuseColour, 1, glm::value_ptr(PointLightSources[i].DiffuseColour));
                glUniform3fv(PointLightSourcesUniform1[i].SpecularColour, 1, glm::value_ptr(PointLightSources[i].SpecularColour));
                glUniform1f(PointLightSourcesUniform1[i].LinearAttenuationCoefficient, PointLightSources[i].LinearAttenuationCoefficient);
                glUniform1f(PointLightSourcesUniform1[i].QuadraticAttenuationCoefficient, PointLightSources[i].QuadraticAttenuationCoefficient);
            }
        #endif
        #if DIRECTIONAL_LIGHT_SOURCES
            for(GLuint i = 0; i < N_DIRECTIONAL_LIGHT_SOURCES; ++i) {
                glUniform3fv(DirectionalLightSourcesUniform1[i].Direction, 1, glm::value_ptr(DirectionalLightSources[i].Direction));
                glUniform3fv(DirectionalLightSourcesUniform1[i].AmbientColour, 1, glm::value_ptr(DirectionalLightSources[i].AmbientColour));
                glUniform3fv(DirectionalLightSourcesUniform1[i].DiffuseColour, 1, glm::value_ptr(DirectionalLightSources[i].DiffuseColour));
                glUniform3fv(DirectionalLightSourcesUniform1[i].SpecularColour, 1, glm::value_ptr(DirectionalLightSources[i].SpecularColour));
            }
            #endif
        #if SPOTLIGHT_LIGHT_SOURCES
        for(GLuint i = 0; i < N_SPOTLIGHT_LIGHT_SOURCES; ++i) {
            glUniform3fv(SpotlightLightSourcesUniform1[i].Position, 1, glm::value_ptr(SpotlightLightSources[i].Position));
            glUniform3fv(SpotlightLightSourcesUniform1[i].Direction, 1, glm::value_ptr(SpotlightLightSources[i].Direction));
            glUniform3fv(SpotlightLightSourcesUniform1[i].AmbientColour, 1, glm::value_ptr(SpotlightLightSources[i].AmbientColour));
            glUniform3fv(SpotlightLightSourcesUniform1[i].DiffuseColour, 1, glm::value_ptr(SpotlightLightSources[i].DiffuseColour));
            glUniform3fv(SpotlightLightSourcesUniform1[i].SpecularColour, 1, glm::value_ptr(SpotlightLightSources[i].SpecularColour));
            glUniform1f(SpotlightLightSourcesUniform1[i].InnerCutoffAngleCosine, SpotlightLightSources[i].InnerCutoffAngleCosine);
            glUniform1f(SpotlightLightSourcesUniform1[i].OuterCutoffAngleCosine, SpotlightLightSources[i].OuterCutoffAngleCosine);
            glUniform1f(SpotlightLightSourcesUniform1[i].LinearAttenuationCoefficient, SpotlightLightSources[i].LinearAttenuationCoefficient);
            glUniform1f(SpotlightLightSourcesUniform1[i].QuadraticAttenuationCoefficient, SpotlightLightSources[i].QuadraticAttenuationCoefficient);
        }
        #endif
        #if FLASHLIGHT_LIGHT_SOURCES
        for(GLuint i = 0; i < N_FLASHLIGHT_LIGHT_SOURCES; ++i) {
            glUniform3fv(FlashlightLightSourcesUniform1[i].AmbientColour, 1, glm::value_ptr(FlashlightLightSources[i].AmbientColour));
            glUniform3fv(FlashlightLightSourcesUniform1[i].DiffuseColour, 1, glm::value_ptr(FlashlightLightSources[i].DiffuseColour));
            glUniform3fv(FlashlightLightSourcesUniform1[i].SpecularColour, 1, glm::value_ptr(FlashlightLightSources[i].SpecularColour));
            glUniform1f(FlashlightLightSourcesUniform1[i].InnerCutoffAngleCosine, FlashlightLightSources[i].InnerCutoffAngleCosine);
            glUniform1f(FlashlightLightSourcesUniform1[i].OuterCutoffAngleCosine, FlashlightLightSources[i].OuterCutoffAngleCosine);
            glUniform1f(FlashlightLightSourcesUniform1[i].LinearAttenuationCoefficient, FlashlightLightSources[i].LinearAttenuationCoefficient);
            glUniform1f(FlashlightLightSourcesUniform1[i].QuadraticAttenuationCoefficient, FlashlightLightSources[i].QuadraticAttenuationCoefficient);
        }
        #endif
        glUniform1i(MaterialUniform1.AmbientReflectivenessMap, 0);
        glUniform1i(MaterialUniform1.DiffuseReflectivenessMap, 0);
        glUniform1i(MaterialUniform1.SpecularReflectivenessMap, 1);
        GLuint ReflectivenessMaps[2u];
        glGenTextures(2, ReflectivenessMaps);
        glActiveTexture(GL_TEXTURE0);
        SetupTexture2D(ReflectivenessMaps[0u], RESOURCE_DIR "container2.png");
        glActiveTexture(GL_TEXTURE1);
        SetupTexture2D(ReflectivenessMaps[1u], RESOURCE_DIR "container2_specular.png");
    #elif POINT_LIGHT_SOURCES || SPOTLIGHT_LIGHT_SOURCES
        glUseProgram(ShaderProgram2);
    #endif
    double FrameStartTime, PrevFrameStartTime = 0.0;
    float DeltaTime;
    bool FirstFrame = true;
    glfwSetTime(0.0);
    while(!glfwWindowShouldClose(Window)) {
        FrameStartTime = glfwGetTime();
        DeltaTime = static_cast<float>(FrameStartTime - PrevFrameStartTime);
        PrevFrameStartTime = FrameStartTime;
        Idfk:
        bool const W = glfwGetKey(Window, GLFW_KEY_W) != GLFW_RELEASE,
                   S = glfwGetKey(Window, GLFW_KEY_S) != GLFW_RELEASE,
                   A = glfwGetKey(Window, GLFW_KEY_A) != GLFW_RELEASE,
                   D = glfwGetKey(Window, GLFW_KEY_D) != GLFW_RELEASE,
                   Space = glfwGetKey(Window, GLFW_KEY_SPACE) != GLFW_RELEASE,
                   LeftShift = glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE;
        if(!(FirstFrame || W - S || A - D || Space - LeftShift || WindowUserPointerValue.RecentMouseCallback)) {
            glfwWaitEvents();
            if(glfwWindowShouldClose(Window))
                break;
            PrevFrameStartTime = glfwGetTime();
            goto Idfk;
        }
        static float constexpr CameraSpeed = 20.0f;
        if(W - S || A - D || Space - LeftShift)
            ViewMat = glm::translate(glm_mat4_Identity, glm::normalize(glm::vec3(A - D, LeftShift - Space, W - S)) * CameraSpeed * DeltaTime) * ViewMat;
        glm::mat4 const ViewProjMat = ProjMat * ViewMat, ViewInverseMat = glm::inverse(ViewMat);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        #if CUBES
            #if POINT_LIGHT_SOURCES || N_SPOTLIGHT_LIGHT_SOURCES
                glUseProgram(ShaderProgram1);
            #endif
            glUniformMatrix4fv(ViewProjMatUniform1, 1, GL_FALSE, glm::value_ptr(ViewProjMat));
            glUniformMatrix4fv(ViewInverseMatUniform1, 1, GL_FALSE, glm::value_ptr(ViewInverseMat));
            for(GLuint i = 0; i < N_CUBES; ++i) {
                glUniformMatrix4fv(ModelMatUniform1, 1, GL_FALSE, glm::value_ptr(CubeModelMats[i]));
                glUniformMatrix4fv(ModelAdjugateMatUniform1, 1, GL_FALSE, glm::value_ptr(CubeModelAdjugateMats[i]));
                glUniform1f(MaterialUniform1.Shininess, Cubes[i].Material.Shininess);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void const *)0zu);
            }
        #endif
        #if POINT_LIGHT_SOURCES || N_SPOTLIGHT_LIGHT_SOURCES
            #if CUBES
                glUseProgram(ShaderProgram2);
            #endif
            #if POINT_LIGHT_SOURCES
                for(GLuint i = 0; i < N_POINT_LIGHT_SOURCES; ++i) {
                    glUniformMatrix4fv(ModelViewProjMatUniform2, 1, GL_FALSE, glm::value_ptr(ViewProjMat * PointLightSourcesModelMats[i]));
                    glUniform1f(ModelSpacePointSizeUniform2, PointLightSources[i].ModelSpacePointSize);
                    glUniform1f(FalloffUniform2, PointLightSources[i].Falloff);
                    glUniform1i(VolumetrixSizeUniform2, PointLightSources[i].VolumetricSize);
                    glUniform3fv(ColourUniform2, 1, glm::value_ptr(PointLightSources[i].DisplayColour));
                    glDrawArrays(GL_POINTS, 0, 1u);
                }
            #endif
            #if SPOTLIGHT_LIGHT_SOURCES
                for(GLuint i = 0; i < N_SPOTLIGHT_LIGHT_SOURCES; ++i) {
                    glUniform3fv(ColourUniform2, 1, glm::value_ptr(SpotlightLightSources[i].DisplayColour));
                    glUniform1f(ModelSpacePointSizeUniform2, SpotlightLightSources[i].ModelSpacePointSize);
                    glUniform1f(FalloffUniform2, SpotlightLightSources[i].Falloff);
                    glUniform1i(VolumetrixSizeUniform2, SpotlightLightSources[i].VolumetricSize);
                    glUniformMatrix4fv(ModelViewProjMatUniform2, 1, GL_FALSE, glm::value_ptr(ViewProjMat * SpotlightLightSourcesModelMats[i]));
                    glDrawArrays(GL_POINTS, 0, 1u);
                }
            #endif
        #endif
        WindowUserPointerValue.RecentMouseCallback = FirstFrame = false;
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}