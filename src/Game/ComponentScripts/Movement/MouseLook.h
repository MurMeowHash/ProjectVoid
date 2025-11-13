#pragma once

#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class MouseLook : public ObjectComponent {
DEFINE_BASE(void)
private:
    float minVerticalAngle = -89.0f;
    float maxVerticalAngle = 89.0f;
    float sensitivityX = 0.1f;
    float sensitivityY = 0.1f;
public:
    explicit MouseLook();
    explicit MouseLook(const MouseLookParameters& params);
    void Update() override;
    
    void SetMinVerticalAngle(float angle) { minVerticalAngle = angle; }
    void SetMaxVerticalAngle(float angle) { maxVerticalAngle = angle; }
    void SetSensitivityX(float sens) { sensitivityX = sens; }
    void SetSensitivityY(float sens) { sensitivityY = sens; }
    
    nlohmann::json SerializeToJson() const override;
    GET_COMPONENT_TYPE_NAME(MouseLook)
    static MouseLook* CreateFromJson(GameObject* owner, const nlohmann::json& params);
};
