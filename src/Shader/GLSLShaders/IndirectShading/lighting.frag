#version 420 core

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define SHININESS_CONVERSION_COEF 128.0

in LightData {
    vec4 PositionType;
    vec4 ColorIntensity;
    vec4 DirectionRadius;
    vec3 AttenuationCoefs;
    vec2 CutOffs;
} lightData;

struct Material {
    vec3 albedo;
    float specular;
    float shininess;
} material;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormalShininess;
uniform sampler2D gAlbedoSpec;
uniform vec3 viewPos;
uniform vec2 viewportSize;

const float volumeEdgesThreshold = 0.999;

vec3 GetDiffuseColor(vec3 lightDir, vec3 normal, vec3 lightColor);
vec3 GetSpecularColor(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 lightColor);
vec3 CalculateLightImpact(vec3 worldPos, vec3 normal, vec3 viewDir);
vec3 CalculateDirectionalLight(vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(vec3 worldPos, vec3 normal, vec3 viewDir);
vec3 CalculateSpotLight(vec3 worldPos, vec3 normal, vec3 viewDir);

void main() {
    vec2 texCoords = gl_FragCoord.xy / viewportSize;
    vec3 worldPos = texture(gPosition, texCoords).rgb;

    float distanceRatio = length(worldPos - lightData.PositionType.xyz) / lightData.DirectionRadius.w;
    if(distanceRatio > 1.0) {
        discard;
    }

    float softEdgesIntensity = clamp((1.0 - distanceRatio) / (1.0 - volumeEdgesThreshold), 0.0, 1.0);

    vec4 normalShininess = texture(gNormalShininess, texCoords);
    vec4 albedoSpec = texture(gAlbedoSpec, texCoords);
    material.albedo = albedoSpec.rgb;
    material.specular = albedoSpec.a;
    material.shininess = normalShininess.w * SHININESS_CONVERSION_COEF;
    vec3 viewDir = normalize(viewPos - worldPos);

    vec3 pixelColor = CalculateLightImpact(worldPos, normalShininess.xyz, viewDir);
    FragColor = vec4(pixelColor * softEdgesIntensity, 1.0);
}

vec3 GetDiffuseColor(vec3 lightDir, vec3 normal, vec3 lightColor) {
    float diffuseIntensity = max(dot(lightDir, normal), 0.0);
    return diffuseIntensity * lightColor * material.albedo;
}

vec3 GetSpecularColor(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 lightColor) {
    vec3 halfDir = normalize(lightDir + viewDir);
    float specularIntensity = pow(max(dot(halfDir, normal), 0.0), material.shininess);
    return specularIntensity * material.specular * lightColor;
}

vec3 CalculateLightImpact(vec3 worldPos, vec3 normal, vec3 viewDir) {
    int lightType = int(round(lightData.PositionType.w));
    switch(lightType) {
        case DIRECTIONAL_LIGHT:
            return CalculateDirectionalLight(normal, viewDir);
        case POINT_LIGHT:
            return CalculatePointLight(worldPos, normal, viewDir);
        case SPOT_LIGHT:
            return CalculateSpotLight(worldPos, normal, viewDir);
    }
    return vec3(0.0);
}

vec3 CalculateDirectionalLight(vec3 normal, vec3 viewDir) {
    vec3 lightColor = lightData.ColorIntensity.rgb * lightData.ColorIntensity.a;
    vec3 lightDir = normalize(-lightData.DirectionRadius.xyz);
    vec3 diffuse = GetDiffuseColor(lightDir, normal, lightColor);
    vec3 specular = GetSpecularColor(lightDir, viewDir, normal, lightColor);

    return diffuse + specular;
}

vec3 CalculatePointLight(vec3 worldPos, vec3 normal, vec3 viewDir) {
    vec3 lightColor = lightData.ColorIntensity.rgb * lightData.ColorIntensity.a;
    vec3 fragToLight = lightData.PositionType.xyz - worldPos;
    vec3 lightDir = normalize(fragToLight);
    vec3 diffuse = GetDiffuseColor(lightDir, normal, lightColor);
    vec3 specular = GetSpecularColor(lightDir, viewDir, normal, lightColor);

    float distance = length(fragToLight);
    float attenuationIntensity = 1.0 / (lightData.AttenuationCoefs.x + lightData.AttenuationCoefs.y * distance
                                                                + lightData.AttenuationCoefs.z * distance * distance);
    diffuse *= attenuationIntensity;
    specular *= attenuationIntensity;

    return diffuse + specular;
}

vec3 CalculateSpotLight(vec3 worldPos, vec3 normal, vec3 viewDir) {
    vec3 pointLightColor = CalculatePointLight(worldPos, normal, viewDir);

    vec3 rayDir = normalize(lightData.PositionType.xyz - worldPos);
    float theta = dot(normalize(-lightData.DirectionRadius.xyz), rayDir);
    float cutOffIntensity = clamp((theta - lightData.CutOffs.y) / (lightData.CutOffs.x - lightData.CutOffs.y), 0.0, 1.0);
    return pointLightColor * cutOffIntensity;
}