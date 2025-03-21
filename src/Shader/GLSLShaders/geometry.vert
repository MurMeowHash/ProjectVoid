#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT {
    vec2 TexCoords;
} vs_out;

layout(std140, binding = 0) uniform TransformMatrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 normal;

void main() {
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}