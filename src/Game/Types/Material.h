#pragma once
#include <string>
#include "../../Utils/Utils.h"

struct Material {
    static constexpr float DEFAULT_SHININESS = 32.0f;
    static constexpr int ABSENT_TEXTURE = -1;

    std::string name = "Undefined";
    int albedoMap = ABSENT_TEXTURE;
    int normalMap = ABSENT_TEXTURE;
    int specularMap = ABSENT_TEXTURE;
    float shininess = DEFAULT_SHININESS;
};