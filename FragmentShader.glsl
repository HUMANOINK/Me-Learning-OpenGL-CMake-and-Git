#version 330 core
in vec2 TextureCoord;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
layout(location = 0) out vec4 OutputColour;
void main() {
    vec4 Tex2Colour = texture(Texture2, TextureCoord);
    OutputColour = mix(texture(Texture1, TextureCoord), Tex2Colour, Tex2Colour.a);
}