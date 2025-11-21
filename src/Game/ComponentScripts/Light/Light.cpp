#include "Light.h"
#include "../../Types/GameObject/GameObject.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <imgui/imgui.h>
#include <glm/glm.hpp>
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

void Light::RenderUI(GameObject* obj) {
    ImGui::Text("Type:");
    ImGui::SetNextItemWidth(-1);
    const char* lightTypes[] = { "Directional", "Point", "Spot" };
    int currentType = static_cast<int>(type);
    if(ImGui::Combo("##LightType", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes))) {
        type = static_cast<LightType>(currentType);
    }

    ImGui::Text("Color:");
    ImGui::SetNextItemWidth(-1);
    float colorArray[3] = { color.x, color.y, color.z };
    if(ImGui::ColorEdit3("##LightColor", colorArray)) {
        color = glm::vec3(colorArray[0], colorArray[1], colorArray[2]);
    }

    ImGui::Text("Intensity:");
    ImGui::SetNextItemWidth(-1);
    if(ImGui::InputFloat("##LightIntensity", &intensity)) {
        if(intensity < 0.0f) intensity = 0.0f;
    }

    if(type == LightType::Point || type == LightType::Spot) {
        ImGui::Text("Radius:");
        ImGui::SetNextItemWidth(-1);
        if(ImGui::InputFloat("##LightRadius", &radius)) {
            if(radius < 0.0f) radius = 0.0f;
        }
    }

    if(type == LightType::Spot) {
        ImGui::Text("Inner CutOff:");
        ImGui::SetNextItemWidth(-1);
        if(ImGui::InputFloat("##LightInnerCutOff", &innerCutOff)) {
            if(innerCutOff < 0.0f) innerCutOff = 0.0f;
            if(innerCutOff > outerCutOff) innerCutOff = outerCutOff;
        }

        ImGui::Text("Outer CutOff:");
        ImGui::SetNextItemWidth(-1);
        if(ImGui::InputFloat("##LightOuterCutOff", &outerCutOff)) {
            if(outerCutOff < innerCutOff) outerCutOff = innerCutOff;
        }
    }
    
    ImGui::Spacing();
}

REGISTER_COMPONENT_FROM_JSON(Light)