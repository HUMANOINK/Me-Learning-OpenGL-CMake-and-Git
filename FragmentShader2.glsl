#version 330 core
uniform vec3 Colour;
layout(location = 0) out vec4 OutputColour;
void main() {
    OutputColour = vec4(Colour, 1.0f);
}