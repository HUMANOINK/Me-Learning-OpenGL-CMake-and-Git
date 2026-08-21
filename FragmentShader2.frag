#version 330 core
uniform vec3 Colour;
uniform float Falloff;
layout(location = 0) out vec4 OutputColour;
void main() {
    float RadialDistance = length(gl_PointCoord * 2.0 - 1.0);
    if(RadialDistance >= 1.0)
        discard;
    OutputColour = vec4(Colour, exp(-Falloff * RadialDistance * RadialDistance));
}