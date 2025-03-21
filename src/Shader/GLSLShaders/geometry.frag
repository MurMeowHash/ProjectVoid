#version 420 core

in VS_OUT {
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

void main() {
    FragColor = vec4(texture(albedoMap, fs_in.TexCoords).rgb, 1.0);
}