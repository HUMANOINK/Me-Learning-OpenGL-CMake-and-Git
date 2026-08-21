#version 460 core
layout(location = 0) in vec2 Position;
layout(location = 1) in vec3 VertexColour;
out vec3 Colour;
void main() {
    gl_Position = vec4(Position, 0.0, 1.0);
    Colour = VertexColour;
}