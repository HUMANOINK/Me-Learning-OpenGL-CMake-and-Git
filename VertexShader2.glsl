#version 330 core
layout(location = 0) in vec3 ModelSpaceVertexPosition;
uniform mat4 ModelViewProjMat;
void main() {
    gl_Position = ModelViewProjMat * vec4(ModelSpaceVertexPosition, 1.0);
}