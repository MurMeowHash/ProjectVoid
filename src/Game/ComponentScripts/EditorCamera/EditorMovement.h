#pragma once

#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>
#include <glm/glm.hpp>

class GameObject;

class EditorMovement : public ObjectComponent {
DEFINE_BASE(void)
private:
    float sensitivityX;
    float sensitivityY;
    float moveSpeed;
    float fastMoveSpeed;
    float slowMoveSpeed;
    float smoothing;
    float zoomSpeed;
    float minZoomSpeed;
    float maxZoomSpeed;
    float currentYaw;
    float currentPitch;
    bool isRightMouseDown;

    void HandleMovement() const;
    void HandleCameraRotation();
    void HandleZoom();

    NODISCARD glm::vec3 GetMouseWheelMovementDirection() const;
    NODISCARD glm::vec3 GetKeysMovementDirection() const;
    NODISCARD glm::vec3 GetMovementDirection() const;
public:
    explicit EditorMovement(const EditorMovementParameters& params = EditorMovementParameters());

    void Start() override;
    void Update() override;

    void SetSensitivityX(float sens);
    void SetSensitivityY(float sens);
    void SetMoveSpeed(float speed);
    void SetFastMoveSpeed(float speed);
    void SetSlowMoveSpeed(float speed);
    void SetSmoothing(float smooth);
    void SetZoomSpeed(float speed);

    NODISCARD float GetSensitivityX() const;
    NODISCARD float GetSensitivityY() const;
    NODISCARD float GetMoveSpeed() const;
    NODISCARD float GetFastMoveSpeed() const;
    NODISCARD float GetSlowMoveSpeed() const;
    NODISCARD float GetSmoothing() const;
    NODISCARD float GetZoomSpeed() const;

    NODISCARD nlohmann::json SerializeToJson() const override;
    static EditorMovement* CreateFromJson(GameObject* owner, const nlohmann::json& params);
    GET_COMPONENT_TYPE_NAME(EditorMovement)
};