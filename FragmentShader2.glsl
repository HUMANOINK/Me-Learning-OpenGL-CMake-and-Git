#version 330 core
in vec2 TextureCoord;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform float Blend;
layout(location = 0) out vec4 OutputColour;
void main() {
    OutputColour = mix(texture(Texture1, TextureCoord), texture(Texture2, TextureCoord), Blend);
}