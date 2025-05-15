#version 420 core

#define DIRECTIONAL_LIGHT_TYPE 0

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 5) in vec4 aLightPositionType;
layout(location = 6) in vec4 aLightColorIntensity;
layout(location = 7) in vec4 aLightDirectionRadius;
layout(location = 8) in vec3 aLightAttenuationCoefs;
layout(location = 9) in vec2 aLightCutOffs;
layout(location = 10) in mat4 aLightVolumeModelMatrix;

out LightData {
    vec4 PositionType;
    vec4 ColorIntensity;
    vec4 DirectionRadius;
    vec3 AttenuationCoefs;
    vec2 CutOffs;
} lightData;

layout(std140, binding = 0) uniform TransformMatrices {
    mat4 projection;
    mat4 view;
};

void main() {
    lightData.PositionType = aLightPositionType;
    lightData.ColorIntensity = aLightColorIntensity;
    lightData.DirectionRadius = aLightDirectionRadius;
    lightData.AttenuationCoefs = aLightAttenuationCoefs;
    lightData.CutOffs = aLightCutOffs;

    if(int(round(aLightPositionType.w)) == DIRECTIONAL_LIGHT_TYPE) {
        gl_Position = vec4(aPos, 1.0);
    }
    else {
        gl_Position = projection * view * aLightVolumeModelMatrix * vec4(aPos, 1.0);
    }
}