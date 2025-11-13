#include "Light.h"
#include "../../Types/GameObject/GameObject.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

Light::Light(const LightParameters &params)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 3), type(params.type), color(params.color), intensity(params.intensity),
radius(params.radius), innerCutOff(params.innerCutOff), outerCutOff(params.outerCutOff) {

}

void Light::Update() {

}

Light* Light::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto lightParams = LightParameters();
    SetIfExists(params, "type", lightParams.type);
    SetIfExists(params, "color", lightParams.color);
    SetIfExists(params, "intensity", lightParams.intensity);
    SetIfExists(params, "radius", lightParams.radius);
    SetIfExists(params, "innerCutOff", lightParams.innerCutOff);
    SetIfExists(params, "outerCutOff", lightParams.outerCutOff);
    return owner->AddComponent<Light>(lightParams);
}

nlohmann::json Light::SerializeToJson() const {
    nlohmann::json params;
    params["type"] = static_cast<int>(type);
    params["intensity"] = intensity;
    params["radius"] = radius;
    params["innerCutOff"] = innerCutOff;
    params["outerCutOff"] = outerCutOff;
    params["color"] = nlohmann::json::array({color.x, color.y, color.z});
    return params;
}

REGISTER_COMPONENT_FROM_JSON(Light)