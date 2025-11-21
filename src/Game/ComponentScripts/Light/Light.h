#pragma once
#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

struct Light : public ObjectComponent {
DEFINE_BASE(void)
    static constexpr LightParameters DEFAULT_LIGHT_PARAMS = LightParameters();

    LightType type;
    glm::vec3 color;
    float intensity;
    float radius;
    float innerCutOff;
    float outerCutOff;

    explicit Light(const LightParameters &params = DEFAULT_LIGHT_PARAMS);
    void Update() override;
    void RenderUI(GameObject* obj) override;
    
    NODISCARD nlohmann::json SerializeToJson() const override;
    static Light* CreateFromJson(GameObject* owner, const nlohmann::json& params);
    GET_COMPONENT_TYPE_NAME(Light)
};