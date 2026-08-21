#version 330 core
layout(location = 0) in vec3 ModelSpaceVertexPosition;
layout(location = 1) in vec3 ModelSpaceVertexNormal;
layout(location = 2) in vec2 VertexTextureCoord;
uniform mat4 ModelMat;
uniform mat4 ModelAdjugateMat;
uniform mat4 ViewProjMat;
out vec3 Position;
out vec3 Normal;
out vec2 TextureCoord;
void main() {
    gl_Position = ModelMat * vec4(ModelSpaceVertexPosition, 1.0);
    Position = gl_Position.xyz / gl_Position.w;
    gl_Position = ViewProjMat * gl_Position;
    Normal = (vec4(ModelSpaceVertexNormal, 0.0) * ModelAdjugateMat).xyz;
    TextureCoord = VertexTextureCoord;
}