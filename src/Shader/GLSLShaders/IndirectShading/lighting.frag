#version 420 core

#define MAX_NUM_LIGHTS 200
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define SHININESS_CONVERSION_COEF 128.0

struct Light {
    vec4 positionType;
    vec4 colorIntensity;
    vec3 direction;
    vec3 attenuationCoefs;
    vec2 cutOffs;
};

struct Material {
    vec3 albedo;
    float specular;
    float shininess;
} material;

out vec4 FragColor;

in VertexData {
    vec2 TexCoords;
} vertexData;

layout(std140, binding = 1) uniform Lights {
    Light lights[MAX_NUM_LIGHTS];
};

uniform sampler2D gPosition;
uniform sampler2D gNormalShininess;
uniform sampler2D gAlbedoSpec;
uniform int countLights;
uniform vec3 environmentAmbient;
uniform vec3 viewPos;

vec3 GetDiffuseColor(vec3 lightDir, vec3 normal, vec3 lightColor);
vec3 GetSpecularColor(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 lightColor);
vec3 CalculateLightImpact(Light light, vec3 worldPos, vec3 normal, vec3 viewDir);
vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(Light light, vec3 worldPos, vec3 normal, vec3 viewDir);
vec3 CalculateSpotLight(Light light, vec3 worldPos, vec3 normal, vec3 viewDir);

void main() {
    vec3 worldPos = texture(gPosition, vertexData.TexCoords).rgb;
    vec4 normalShininess = texture(gNormalShininess, vertexData.TexCoords);
    vec4 albedoSpec = texture(gAlbedoSpec, vertexData.TexCoords);
    material.albedo = albedoSpec.rgb;
    material.specular = albedoSpec.a;
    material.shininess = normalShininess.w * SHININESS_CONVERSION_COEF;
    vec3 viewDir = normalize(viewPos - worldPos);

    vec3 pixelColor = vec3(0.0);
    for(int i = 0; i < countLights; i++) {
        pixelColor += CalculateLightImpact(lights[i], worldPos, normalShininess.xyz, viewDir);
    }

    vec3 finalColor = pixelColor + environmentAmbient * material.albedo;
    FragColor = vec4(finalColor, 1.0);
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

vec3 CalculateLightImpact(Light light, vec3 worldPos, vec3 normal, vec3 viewDir) {
    int lightType = int(light.positionType.w);
    switch(lightType) {
        case DIRECTIONAL_LIGHT:
            return CalculateDirectionalLight(light, normal, viewDir);
        case POINT_LIGHT:
            return CalculatePointLight(light, worldPos, normal, viewDir);
        case SPOT_LIGHT:
            return CalculateSpotLight(light, worldPos, normal, viewDir);
    }

    return vec3(0.0);
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightColor = light.colorIntensity.rgb * light.colorIntensity.a;
    vec3 lightDir = normalize(-light.direction);
    vec3 diffuse = GetDiffuseColor(lightDir, normal, lightColor);
    vec3 specular = GetSpecularColor(lightDir, viewDir, normal, lightColor);

    return diffuse + specular;
}

vec3 CalculatePointLight(Light light, vec3 worldPos, vec3 normal, vec3 viewDir) {
    vec3 lightColor = light.colorIntensity.rgb * light.colorIntensity.a;
    vec3 fragToLight = light.positionType.xyz - worldPos;
    vec3 lightDir = normalize(fragToLight);
    vec3 diffuse = GetDiffuseColor(lightDir, normal, lightColor);
    vec3 specular = GetSpecularColor(lightDir, viewDir, normal, lightColor);

    float distance = length(fragToLight);
    float attenuationIntensity = 1.0 / (light.attenuationCoefs.x + light.attenuationCoefs.y * distance
                                                                + light.attenuationCoefs.z * distance * distance);
    diffuse *= attenuationIntensity;
    specular *= attenuationIntensity;

    return diffuse + specular;
}

vec3 CalculateSpotLight(Light light, vec3 worldPos, vec3 normal, vec3 viewDir) {
    vec3 pointLightColor = CalculatePointLight(light, worldPos, normal, viewDir);

    vec3 rayDir = normalize(light.positionType.xyz - worldPos);
    float theta = dot(-light.direction, rayDir);
    float cutOffIntensity = clamp((theta - light.cutOffs.y) / (light.cutOffs.x - light.cutOffs.y), 0.0, 1.0);
    return pointLightColor * cutOffIntensity;
}