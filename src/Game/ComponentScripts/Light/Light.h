#pragma once
#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"

struct Light : public ObjectComponent {
    static constexpr LightParameters DEFAULT_LIGHT_PARAMS = LightParameters();

    LightType type;
    glm::vec3 color;
    float intensity;
    float radius;
    float innerCutOff;
    float outerCutOff;

    explicit Light(const LightParameters &params = DEFAULT_LIGHT_PARAMS);
    void Update() override;
};