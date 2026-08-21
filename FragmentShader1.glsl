#version 330 core
struct PointLightSource_t {
    vec4 Position;
    vec3 AmbientColour;
    vec3 DiffuseColour;
    vec3 SpecularColour;
};
struct Material_t {
    vec3 AmbientReflectiveness;
    vec3 DiffuseReflectiveness;
    vec3 SpecularReflectiveness;
    float Shininess;
};
in vec3 Position;
in vec3 Normal;
in vec2 TextureCoord;
uniform mat4 ViewInverseMat;
uniform PointLightSource_t PointLightSource;
uniform Material_t Material;
layout(location = 0) out vec4 OutputColour;
void main() {
    vec3 NormalDir = normalize(Normal);
    vec3 LightDir = normalize(PointLightSource.Position.xyz - PointLightSource.Position.w * Position);
    vec4 CameraPos = ViewInverseMat * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 CameraDir = normalize(CameraPos.xyz - CameraPos.w * Position);
    vec3 ReflectDir = reflect(-LightDir, NormalDir);
    OutputColour = vec4(
        PointLightSource.AmbientColour * Material.AmbientReflectiveness
      + 0.5 * (dot(NormalDir, LightDir) + 1.0) * PointLightSource.DiffuseColour * Material.DiffuseReflectiveness
      + pow(0.5 *(dot(CameraDir, ReflectDir) + 1.0), Material.Shininess) * PointLightSource.SpecularColour * Material.SpecularReflectiveness
    , 1.0);
}