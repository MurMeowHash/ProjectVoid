#include "MouseLook.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

MouseLook::MouseLook()
: ObjectComponent(0) {

}

MouseLook::MouseLook(const MouseLookParameters& params)
: ObjectComponent(0), minVerticalAngle(params.minVerticalAngle), maxVerticalAngle(params.maxVerticalAngle),
  sensitivityX(params.sensitivityX), sensitivityY(params.sensitivityY) {

}

void MouseLook::Update() {
    auto transform = GetGameObject()->GetComponent<Transform>();

    transform->rotation.x += Input::GetMouseOffsetY() * sensitivityX;
    transform->rotation.y -= Input::GetMouseOffsetX() * sensitivityY;

    transform->rotation.x = glm::clamp(transform->rotation.x, minVerticalAngle, maxVerticalAngle);
}

MouseLook* MouseLook::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto mouseLookParams = MouseLookParameters();
    SetIfExists(params, "minVerticalAngle", mouseLookParams.minVerticalAngle);
    SetIfExists(params, "maxVerticalAngle", mouseLookParams.maxVerticalAngle);
    SetIfExists(params, "sensitivityX", mouseLookParams.sensitivityX);
    SetIfExists(params, "sensitivityY", mouseLookParams.sensitivityY);
    return owner->AddComponent<MouseLook>(mouseLookParams);
}

nlohmann::json MouseLook::SerializeToJson() const {
    nlohmann::json params;
    params["minVerticalAngle"] = minVerticalAngle;
    params["maxVerticalAngle"] = maxVerticalAngle;
    params["sensitivityX"] = sensitivityX;
    params["sensitivityY"] = sensitivityY;
    return params;
}

REGISTER_COMPONENT_FROM_JSON(MouseLook)