#version 420 core

in VertexData {
    vec2 TexCoords;
} vertexData;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float exposure;
uniform float gamma;

void main() {
    vec3 pixelColor = texture(screenTexture, vertexData.TexCoords).rgb;

    pixelColor = vec3(1.0) - exp(-pixelColor * exposure);
    pixelColor = pow(pixelColor, vec3(1.0 / gamma));
    FragColor = vec4(pixelColor, 1.0);
}