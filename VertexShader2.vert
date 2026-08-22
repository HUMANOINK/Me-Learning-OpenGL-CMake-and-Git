#version 330 core
uniform mat4 ModelViewProjMat;
uniform float ModelSpacePointSize;
uniform bool VolumetricSize;
#define SubexpressionOfDifferential (ModelViewProjMat[3].w * mat3(ModelViewProjMat) - outerProduct(ModelViewProjMat[3].xyz, vec3(ModelViewProjMat[0].w, ModelViewProjMat[1].w, ModelViewProjMat[2].w)))
float CubeRoot(float Cube) {
    return pow(Cube, 1.0 / 3.0);
}
void main() {
    /*
        Volumetric size calculates change factor in infinitesimal volume, by the MVP transformation, around the pre-transform origin (the model space position of the light source)
        and cube roots it to get the length change factor.
        Alternative calculates change factor in infinitesimal area, by the MVP transformation, parallel to the xy-plane post-transform (the clip space plane parallel to the screen)
        around the pre-transform origin (the model space position of the point light source) and square roots it to get the length change factor.
    */
    if(VolumetricSize)
        gl_PointSize = ModelSpacePointSize * abs(CubeRoot(determinant(SubexpressionOfDifferential)) / (ModelViewProjMat[3].w * ModelViewProjMat[3].w));
    else {
        mat3 InverseDifferential = (ModelViewProjMat[3].w * ModelViewProjMat[3].w) * inverse(SubexpressionOfDifferential);
        gl_PointSize = ModelSpacePointSize * inversesqrt(length(cross(InverseDifferential[0], InverseDifferential[1])));
    }
    gl_Position = ModelViewProjMat[3];
}