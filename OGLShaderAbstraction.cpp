#include <cstdlib>
#include <cstdio>
#include "OGLShaderAbstraction.hpp"
static void PrintLocation(std::source_location const &Location, std::FILE *const File = stdout) {
    std::fprintf(File, "Line: %u\t\tColumn: %u\t\tFunction: %s\t\tFile: %s\n", Location.line(), Location.column(), Location.function_name(), Location.file_name());
}
void CompileShader(GLuint const Shader, std::source_location const &At) {
    GLint ShaderProperty;
    #ifndef NDEBUG_OPENGL
        char *InfoLog;
    #endif
    glCompileShader(Shader);
    #ifndef NDEBUG_OPENGL
    glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &ShaderProperty);
    if (ShaderProperty > 1) [[unlikely]] {
        InfoLog = new char[ShaderProperty];
        glGetShaderInfoLog(Shader, ShaderProperty, nullptr, InfoLog);
    }
    else [[likely]]
        InfoLog = nullptr;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &ShaderProperty);
    if (ShaderProperty == GL_TRUE) [[likely]] {
        if (InfoLog) [[unlikely]] {
            std::printf("Shader compilation succeeded:\n%s\n", InfoLog);
            PrintLocation(At);
        }
    }
    else [[unlikely]] {
        if (InfoLog) [[likely]]
            std::fprintf(stderr, "Shader compilation failed:\n%s\n", InfoLog);
        else [[unlikely]]
            std::fprintf(stderr, "Shader compilation failed!\n");
        PrintLocation(At, stderr);
    }
    delete[] InfoLog;
    #endif
}
void CompileShader(GLuint const Shader, char const *const ShaderFilepath, std::source_location const &At) {
    std::FILE *const SourceFile = std::fopen(ShaderFilepath, "rb");
    if (!SourceFile) [[unlikely]] {
        std::fprintf(stderr, "Error opening shader file \"%s\"!\n", ShaderFilepath);
        std::perror(nullptr);
        PrintLocation(At, stderr);
        std::exit(EXIT_FAILURE);
    }
    if (std::fseek(SourceFile, 0, SEEK_END)) [[unlikely]] {
        std::fprintf(stderr, "Error seeking to end of shader file \"%s\" to get its length!\n", ShaderFilepath);
        std::perror(nullptr);
        PrintLocation(At, stderr);
        std::exit(EXIT_FAILURE);
    }
    GLint const Length = static_cast<GLint>(std::ftell(SourceFile));
    if (Length <= 0) [[unlikely]] {
        std::fprintf(stderr, "Error! Shader file \"%s\" is either empty or too big!\n", ShaderFilepath);
        PrintLocation(At, stderr);
        std::exit(EXIT_FAILURE);
    }
    std::rewind(SourceFile);
    GLchar *const Source = new GLchar[Length];//OpenGL guarantees sizeof(GLchar) is 1 byte
    if (static_cast<GLuint>(std::fread(Source, sizeof(GLchar), Length, SourceFile)) < static_cast<GLuint>(Length)) [[unlikely]] {
        std::fprintf(stderr, "Error reading from shader file \"%s\"!\n", ShaderFilepath);
        std::perror(nullptr);
        PrintLocation(At, stderr);
        std::exit(EXIT_FAILURE);
    }
    std::fclose(SourceFile);
    glShaderSource(Shader, 1, &Source, &Length);
    delete[] Source;
    CompileShader(Shader, At);
}
GLuint CreateCompiledShader(GLenum ShaderType, char const *ShaderFilepath, std::source_location const &At) {
    GLuint const Shader = glCreateShader(ShaderType);
    CompileShader(Shader, ShaderFilepath, At);
    return Shader;
}
void LinkShaderProgram(GLuint const ShaderProgram, std::source_location const &At) {
    glLinkProgram(ShaderProgram);
    #ifndef NDEBUG_OPENGL
        GLint ProgramProperty;
        char *InfoLog;
        glGetProgramiv(ShaderProgram, GL_INFO_LOG_LENGTH, &ProgramProperty);
        if (ProgramProperty > 1) [[unlikely]] {
            InfoLog = new char[ProgramProperty];
            glGetProgramInfoLog(ShaderProgram, ProgramProperty, nullptr, InfoLog);
        }
        else [[likely]]
            InfoLog = nullptr;
        glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &ProgramProperty);
        if (ProgramProperty == GL_TRUE) [[likely]] {
            if (InfoLog) [[unlikely]] {
                std::printf("Shader program linking succeeded:\n%s\n", InfoLog);
                PrintLocation(At);
            }
        }
        else [[unlikely]] {
            if (InfoLog) [[likely]]
                std::fprintf(stderr, "Shader program linking failed:\n%s\n", InfoLog);
            else [[unlikely]]
                std::fprintf(stderr, "Shader program linking failed!\n");
            PrintLocation(At, stderr);
        }
        delete[] InfoLog;
    #endif
    glValidateProgram(ShaderProgram);
    #ifndef NDEBUG_OPENGL
        glGetProgramiv(ShaderProgram, GL_INFO_LOG_LENGTH, &ProgramProperty);
        if (ProgramProperty > 1) [[unlikely]] {
            InfoLog = new char[ProgramProperty];
            glGetProgramInfoLog(ShaderProgram, ProgramProperty, nullptr, InfoLog);
        }
        else [[likely]]
            InfoLog = nullptr;
        glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &ProgramProperty);
        if (ProgramProperty == GL_TRUE) [[likely]] {
            if (InfoLog) [[unlikely]] {
                std::printf("Shader program validation succeeded:\n%s\n", InfoLog);
                PrintLocation(At);
            }
        }
        else [[unlikely]] {
            if (InfoLog) [[likely]]
                std::fprintf(stderr, "Shader program validation failed:\n%s\n", InfoLog);
            else [[unlikely]]
                std::fprintf(stderr, "Shader program validation failed!\n");
            PrintLocation(At, stderr);
            std::exit(EXIT_FAILURE);
        }
        delete[] InfoLog;
    #endif
}
GLuint CreateLinkedShaderProgram(GLuint const VertexShader, GLuint const FragmentShader, std::source_location const &At) {
    GLuint const ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, VertexShader);
    glAttachShader(ShaderProgram, FragmentShader);
    LinkShaderProgram(ShaderProgram, At);
    glDetachShader(ShaderProgram, VertexShader);
    glDetachShader(ShaderProgram, FragmentShader);
    return ShaderProgram;
}
GLuint CreateLinkedShaderProgram(char const *VertexShaderFilepath, char const *FragmentShaderFilepath, std::source_location const &At) {
    GLuint const VertexShader = CreateCompiledShader(GL_VERTEX_SHADER, VertexShaderFilepath, At),
               FragmentShader = CreateCompiledShader(GL_FRAGMENT_SHADER, FragmentShaderFilepath, At);
    GLuint const ShaderProgram = CreateLinkedShaderProgram(VertexShader, FragmentShader, At);
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    return ShaderProgram;
}
GLint GetUniformLocation(GLuint const Program, GLchar const *const UniformName) {
    GLint Location = glGetUniformLocation(Program, UniformName);
    #ifndef NDEBUG_OPENGL
        if(Location < 0) [[unlikely]]
            std::fprintf(stderr, "Failed to retrieve location of uniform \"%s\"\n", UniformName);
    #endif
    return Location;
}