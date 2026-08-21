#include <source_location>
#include <glad/gl.h>
void CompileShader(GLuint Shader, std::source_location const &At = std::source_location::current());
void CompileShader(GLuint Shader, char const *ShaderFilepath, std::source_location const &At = std::source_location::current());
GLuint CreateCompiledShader(GLenum ShaderType, char const *ShaderFilepath, std::source_location const &At = std::source_location::current());
void LinkShaderProgram(GLuint ShaderProgram, std::source_location const &At = std::source_location::current());
GLuint CreateLinkedShaderProgram(GLuint VertexShader, GLuint FragmentShader, std::source_location const &At = std::source_location::current());
GLuint CreateLinkedShaderProgram(char const *VertexShaderFilepath, char const *FragmentShaderFilepath, std::source_location const &At = std::source_location::current());
GLint GetUniformLocation(GLuint Program, GLchar const *UniformName, std::source_location const &At = std::source_location::current());