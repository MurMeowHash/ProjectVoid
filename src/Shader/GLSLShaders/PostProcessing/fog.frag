#version 420 core

in VertexData {
    vec2 TexCoords;
} vertexData;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D screenTexture;
uniform vec3 viewPos;
uniform vec3 fogProperties;
uniform vec3 fogColor;

float GetFogStart();
float GetFogEnd();

void main() {
    vec3 worldPos = texture(gPosition, vertexData.TexCoords).rgb;
    vec3 sceneColor = texture(screenTexture, vertexData.TexCoords).rgb;
    float distanceToFrag = length(worldPos - viewPos);

    float fogFactor = clamp((distanceToFrag - GetFogStart()) / (GetFogEnd() - GetFogStart()), 0.0, 1.0);
    vec3 blendedColor = mix(sceneColor, fogColor, fogFactor);
    FragColor = vec4(blendedColor, 1.0f);
}

float GetFogStart() {
    return fogProperties.x;
}

float GetFogEnd() {
    return fogProperties.y;
}