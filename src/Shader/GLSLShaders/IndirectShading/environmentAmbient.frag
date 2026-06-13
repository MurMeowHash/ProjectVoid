#version 420 core

out vec4 FragColor;

in VertexData {
    vec2 TexCoords;
} vertexData;

uniform sampler2D gAlbedoSpec;
uniform vec3 ambientColor;

void main() {
    vec3 pixelColor = texture(gAlbedoSpec, vertexData.TexCoords).rgb;
    FragColor = vec4(pixelColor * ambientColor, 1.0);
}