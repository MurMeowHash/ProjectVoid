#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

out GeomData {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 BiTangent;
} geomData;

layout(std140, binding = 0) uniform TransformMatrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 normal;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    geomData.WorldPos = vec3(worldPos);
    geomData.Normal = normal * aNormal;
    geomData.TexCoords = aTexCoords;
    geomData.Tangent = normal * aTangent;
    geomData.BiTangent = normal * aBiTangent;

    gl_Position = projection * view * worldPos;
}