#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

out VertexData {
    vec2 TexCoords;
} vertexData;

void main() {
    vertexData.TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}