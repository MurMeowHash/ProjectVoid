#include "Material.h"

Material::Material(std::string name, int albedo, int normal, int specular, float shin)
: name(std::move(name)), albedoMap{albedo}, normalMap{normal}, specularMap{specular} {
    SetShininess(shin);
}

void Material::SetAlbedoMap(int mapIndex) {
    albedoMap = mapIndex;
}

void Material::SetNormalMap(int mapIndex) {
    normalMap = mapIndex;
}

void Material::SetSpecularMap(int mapIndex) {
    specularMap = mapIndex;
}

void Material::SetShininess(float shin) {
    shininess = std::clamp(shin, MIN_SHININESS, MAX_SHININESS);
}

int Material::GetAlbedoMap() const {
    return albedoMap;
}

int Material::GetNormalMap() const {
    return normalMap;
}

int Material::GetSpecularMap() const {
    return specularMap;
}

float Material::GetShininess() const {
    return shininess;
}

std::string Material::GetName() const {
    return name;
}

float Material::NormalizeShininess(float rawShininess, const std::string &format) {
    float shin = rawShininess;
    if(format == "obj") {
        shin = rawShininess / OBJ_MAX_SHININESS;
    }
    else if(format == "fbx") {
        shin = rawShininess / FBX_MAX_SHININESS;
    }

    return std::clamp(shin, MIN_SHININESS, MAX_SHININESS);
}
