#pragma once
#include <string>
#include "../../Utils/Utils.h"

class Material {
private:
    static constexpr float MIN_SHININESS = 0.0f;
    static constexpr float MAX_SHININESS = 1.0f;
    static constexpr float OBJ_MAX_SHININESS = 1000.0f;
    static constexpr float FBX_MAX_SHININESS = 128.0f;
    static constexpr float OPENGL_MAX_SHININESS = 128.0f;
    std::string name;
    int albedoMap;
    int normalMap;
    int specularMap;
    float shininess;
public:
    static constexpr float DEFAULT_SHININESS = 0.5f;
    explicit Material(std::string name = UNDEFINED_NAME, int albedo = ABSENT_RESOURCE,
             int normal = ABSENT_RESOURCE, int specular = ABSENT_RESOURCE, float shin = DEFAULT_SHININESS);
    void SetAlbedoMap(int mapIndex);
    void SetNormalMap(int mapIndex);
    void SetSpecularMap(int mapIndex);
    void SetShininess(float shin);

    NODISCARD int GetAlbedoMap() const;
    NODISCARD int GetNormalMap() const;
    NODISCARD int GetSpecularMap() const;
    NODISCARD float GetShininess() const;
    NODISCARD std::string GetName() const;

    static float NormalizeShininess(float rawShininess, const std::string &format);
};
