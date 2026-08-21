#version 460 core
in vec3 Colour;
uniform float Alpha;
layout(location = 0) out vec4 OutputColour;
void main() {
    OutputColour = vec4(Colour, Alpha);
}