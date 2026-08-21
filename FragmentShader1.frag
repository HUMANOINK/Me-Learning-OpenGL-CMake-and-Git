#version 330 core
#define POINT_LIGHT_SOURCES
//#define DIRECTIONAL_LIGHT_SOURCES
//#define SPOTLIGHT_LIGHT_SOURCES
//#define FLASHLIGHT_LIGHT_SOURCES
const uint N_POINT_LIGHT_SOURCES = 3u;
const uint N_DIRECTIONAL_LIGHT_SOURCES = 0u;
const uint N_SPOTLIGHT_LIGHT_SOURCES = 0u;
const uint N_FLASHLIGHT_LIGHT_SOURCES = 0u;
#ifdef POINT_LIGHT_SOURCES
    struct PointLightSource_t {
        vec3 Position;
        vec3 AmbientColour, DiffuseColour, SpecularColour;
        float LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
    };
#endif
#ifdef DIRECTIONAL_LIGHT_SOURCES
    struct DirectionalLightSource_t {
        vec3 Direction;
        vec3 AmbientColour, DiffuseColour, SpecularColour;
    };
#endif
#ifdef SPOTLIGHT_LIGHT_SOURCES
    struct SpotlightLightSource_t {
        vec3 Position, Direction;
        vec3 AmbientColour, DiffuseColour, SpecularColour;
        float InnerCutoffAngleCosine, OuterCutoffAngleCosine;
        float LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
    };
#endif
#ifdef FLASHLIGHT_LIGHT_SOURCES
    struct FlashlightLightSource_t {//Just a spotlight, but one particularly known to always be at the camera's position and facing the same way as the camera
        vec3 AmbientColour, DiffuseColour, SpecularColour;
        float InnerCutoffAngleCosine, OuterCutoffAngleCosine;
        float LinearAttenuationCoefficient, QuadraticAttenuationCoefficient;
    };
#endif
struct Material_t {
    sampler2D AmbientReflectivenessMap, DiffuseReflectivenessMap, SpecularReflectivenessMap;
    float Shininess;
};
in vec3 Position;
in vec3 Normal;
in vec2 TextureCoord;
uniform mat4 ViewInverseMat;
#ifdef POINT_LIGHT_SOURCES
    uniform PointLightSource_t PointLightSources[N_POINT_LIGHT_SOURCES];
#endif
#ifdef DIRECTIONAL_LIGHT_SOURCES
    uniform DirectionalLightSource_t DirectionalLightSources[N_DIRECTIONAL_LIGHT_SOURCES];
#endif
#ifdef SPOTLIGHT_LIGHT_SOURCES
    uniform SpotlightLightSource_t SpotlightLightSources[N_SPOTLIGHT_LIGHT_SOURCES];
#endif
#ifdef FLASHLIGHT_LIGHT_SOURCES
    uniform FlashlightLightSource_t FlashlightLightSources[N_FLASHLIGHT_LIGHT_SOURCES];
#endif
uniform Material_t Material;
layout(location = 0) out vec4 OutputColour;
vec3 NormalDir;
float DistToCamera;
vec3 DirToCamera;
vec3 CameraDir;
void InitialiseGlobalVariables() {
    NormalDir = normalize(Normal);
    vec3 CameraRelPos = ViewInverseMat[3].xyz / ViewInverseMat[3].w - Position;//ViewInverseMat * vec4(0.0, 0.0, 0.0, 1.0), as that vec4 represents the origin
    DistToCamera = length(CameraRelPos);
    DirToCamera = normalize(ViewInverseMat[3].xyz - ViewInverseMat[3].w * Position);
    CameraDir = normalize(-ViewInverseMat[2].xyz);//ViewInverseMat * vec4(0.0, 0.0, -1.0, 0.0)
}
#ifdef POINT_LIGHT_SOURCES
    mat3 PointLightSourceContribution(uint Idx) {
        vec3 LightRelPos = PointLightSources[Idx].Position - Position;
        float DistToLight = length(LightRelPos);
        vec3 DirToLight = LightRelPos / DistToLight;
        return mat3(
            PointLightSources[Idx].AmbientColour,
            max(0.0, dot(NormalDir, DirToLight)) * PointLightSources[Idx].DiffuseColour,
            pow(max(0.0, dot(NormalDir, normalize(DirToCamera + DirToLight))), Material.Shininess) * PointLightSources[Idx].SpecularColour
        ) / (1.0 + PointLightSources[Idx].LinearAttenuationCoefficient * DistToLight + PointLightSources[Idx].QuadraticAttenuationCoefficient * DistToLight * DistToLight);
    }
#endif
#ifdef DIRECTIONAL_LIGHT_SOURCES
    mat3 DirectionalLightSourceContribution(uint Idx) {
        return mat3(
            DirectionalLightSources[Idx].AmbientColour,
            max(0.0, dot(NormalDir, -DirectionalLightSources[Idx].Direction)) * DirectionalLightSources[Idx].DiffuseColour,
            pow(max(0.0, dot(NormalDir, normalize(DirToCamera - DirectionalLightSources[Idx].Direction))), Material.Shininess) * DirectionalLightSources[Idx].SpecularColour
        );
    }
#endif
#ifdef SPOTLIGHT_LIGHT_SOURCES
    mat3 SpotlightLightSourceContribution(uint Idx) {
        vec3 LightRelPos = SpotlightLightSources[Idx].Position - Position;
        float DistToLight = length(LightRelPos);
        vec3 DirToLight = LightRelPos / DistToLight;
        return mat3(
            SpotlightLightSources[Idx].AmbientColour,
            max(0.0, dot(NormalDir, DirToLight)) * SpotlightLightSources[Idx].DiffuseColour,
            pow(max(0.0, dot(NormalDir, normalize(DirToCamera + DirToLight))), Material.Shininess) * SpotlightLightSources[Idx].SpecularColour
        ) * clamp((dot(DirToLight, normalize(SpotlightLightSources[Idx].Direction)) + SpotlightLightSources[Idx].OuterCutoffAngleCosine) / (SpotlightLightSources[Idx].OuterCutoffAngleCosine - SpotlightLightSources[Idx].InnerCutoffAngleCosine), 0.0, 1.0)
          / (1.0 + SpotlightLightSources[Idx].LinearAttenuationCoefficient * DistToLight + SpotlightLightSources[Idx].QuadraticAttenuationCoefficient * DistToLight * DistToLight);
    }
#endif
#ifdef FLASHLIGHT_LIGHT_SOURCES
    mat3 FlashlightLightSourceContribution(uint Idx) {
        return mat3(
            FlashlightLightSources[Idx].AmbientColour,
            max(0.0, dot(NormalDir, DirToCamera)) * FlashlightLightSources[Idx].DiffuseColour,
            pow(max(0.0, dot(NormalDir, DirToCamera)), Material.Shininess) * FlashlightLightSources[Idx].SpecularColour
        ) * clamp((dot(DirToCamera, CameraDir) + FlashlightLightSources[Idx].OuterCutoffAngleCosine) / (FlashlightLightSources[Idx].OuterCutoffAngleCosine - FlashlightLightSources[Idx].InnerCutoffAngleCosine), 0.0, 1.0)
          / (1.0 + FlashlightLightSources[Idx].LinearAttenuationCoefficient * DistToCamera + FlashlightLightSources[Idx].QuadraticAttenuationCoefficient * DistToCamera * DistToCamera);
    }
#endif
void main() {
    InitialiseGlobalVariables();
    mat3 Lighting = mat3(0.0);
    #ifdef POINT_LIGHT_SOURCES
        for(uint i = 0u; i < N_POINT_LIGHT_SOURCES; ++i)
            Lighting += PointLightSourceContribution(i);
    #endif
    #ifdef DIRECTIONAL_LIGHT_SOURCES
        for(uint i = 0u; i < N_DIRECTIONAL_LIGHT_SOURCES; ++i)
            Lighting += DirectionalLightSourceContribution(i);
    #endif
    #ifdef SPOTLIGHT_LIGHT_SOURCES
        for(uint i = 0u; i < N_SPOTLIGHT_LIGHT_SOURCES; ++i)
            Lighting += SpotlightLightSourceContribution(i);
    #endif
    #ifdef FLASHLIGHT_LIGHT_SOURCES
        for(uint i = 0u; i < N_FLASHLIGHT_LIGHT_SOURCES; ++i)
            Lighting += FlashlightLightSourceContribution(i);
    #endif
    OutputColour = vec4(
        Lighting[0] * texture(Material.AmbientReflectivenessMap, TextureCoord).rgb
      + Lighting[1] * texture(Material.DiffuseReflectivenessMap, TextureCoord).rgb
      + Lighting[2] * texture(Material.SpecularReflectivenessMap, TextureCoord).rgb,
    1.0);
}