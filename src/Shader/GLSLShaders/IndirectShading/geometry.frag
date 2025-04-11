#version 420 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormalShininess;
layout(location = 2) out vec4 gAlbedoSpec;

in GeomData {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 BiTangent;
} geomData;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform float shininess;
uniform bool hasAlbedoMap;
uniform bool hasNormalMap;
uniform bool hasSpecularMap;

void main() {
    vec3 outAlbedo = vec3(0.5);
    if(hasAlbedoMap) {
        outAlbedo = texture(albedoMap, geomData.TexCoords).rgb;
    }

    vec3 outNormal = normalize(geomData.Normal);
    if(hasNormalMap) {
        mat3 tbn = mat3(normalize(geomData.Tangent), normalize(geomData.BiTangent), outNormal);
        outNormal = normalize(tbn * (texture(normalMap, geomData.TexCoords).rgb * 2.0 - 1.0));
    }

    float specular = 0.5;
    if(hasSpecularMap) {
        specular = texture(specularMap, geomData.TexCoords).r;
    }

    gPosition = vec4(geomData.WorldPos, 1.0);
    gNormalShininess = vec4(outNormal, shininess);
    gAlbedoSpec = vec4(outAlbedo, specular);
}