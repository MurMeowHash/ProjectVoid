#include "EditorCamera.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include "../../../Engine/Time/Time.h"
#include "../Transform/Transform.h"
#include <nlohmann/json.hpp>

EditorCamera::EditorCamera(const EditorCameraParameters& params)
    : ObjectComponent(0),
      sensitivityX(params.sensitivityX),
      sensitivityY(params.sensitivityY),
      moveSpeed(params.moveSpeed),
      fastMoveSpeed(params.fastMoveSpeed),
      slowMoveSpeed(params.slowMoveSpeed),
      smoothing(params.smoothing),
      zoomSpeed(params.zoomSpeed),
      minZoomSpeed(params.minZoomSpeed),
      maxZoomSpeed(params.maxZoomSpeed),
      currentYaw(0.0f),
      currentPitch(0.0f),
      isLeftMouseDown(false) {
}

void EditorCamera::Start() {
    auto* transform = GetGameObject()->GetComponent<Transform>();

    if(transform) {
        currentYaw = transform->rotation.y;
        currentPitch = transform->rotation.x;
    }
}

void EditorCamera::Update() {
    HandleCameraRotation();
    HandleZoom();
}

void EditorCamera::HandleMovement() {

}

void EditorCamera::HandleCameraRotation() {
    auto* transform = GetGameObject()->GetComponent<Transform>();
    if(!transform) return;

    // Обертання камери на ліву кнопку миші
    if(Input::GetMouseButton(Input::MouseButton::MouseLeft)) {
        if(!isLeftMouseDown) {
            isLeftMouseDown = true;
        }

        // Отримуємо зміщення миші
        float mouseX = Input::GetMouseOffsetX() * sensitivityX;
        float mouseY = Input::GetMouseOffsetY() * sensitivityY;

        // Оновлюємо кути
        currentYaw -= mouseX;
        currentPitch += mouseY;

        // Обмежуємо pitch
        currentPitch = glm::clamp(currentPitch, -89.0f, 89.0f);

        // Застосовуємо обертання
        transform->rotation.x = currentPitch;
        transform->rotation.y = currentYaw;
    } else {
        if(isLeftMouseDown) {
            isLeftMouseDown = false;
        }
    }
}

void EditorCamera::HandleZoom() {
    auto* transform = GetGameObject()->GetComponent<Transform>();
    if(!transform) return;

    float scrollDelta = Input::GetScrollOffset();

    if(scrollDelta != 0.0f) {
        glm::vec3 forward = glm::normalize(transform->ToForwardVector());
        float deltaTime = Time::GetDeltaTime();

        float zoomAmount = scrollDelta * zoomSpeed;
        zoomAmount = glm::clamp(zoomAmount, -maxZoomSpeed, maxZoomSpeed);

        transform->position += forward * zoomAmount * deltaTime * 10.0f;
    }
}

void EditorCamera::SetSensitivityX(float sens) {
    sensitivityX = sens;
}

void EditorCamera::SetSensitivityY(float sens) {
    sensitivityY = sens;
}

void EditorCamera::SetMoveSpeed(float speed) {
    moveSpeed = speed;
}
void EditorCamera::SetFastMoveSpeed(float speed) {
    fastMoveSpeed = speed;
}
void EditorCamera::SetSlowMoveSpeed(float speed) {
    slowMoveSpeed = speed;
}

void EditorCamera::SetSmoothing(float smooth) {
    smoothing = smooth;
}

void EditorCamera::SetZoomSpeed(float speed) {
    zoomSpeed = speed;
}

float EditorCamera::GetSensitivityX() const {
    return sensitivityX;
}

float EditorCamera::GetSensitivityY() const {
    return sensitivityY;
}

float EditorCamera::GetMoveSpeed() const {
    return moveSpeed;
}

float EditorCamera::GetFastMoveSpeed() const {
    return fastMoveSpeed;
}

float EditorCamera::GetSlowMoveSpeed() const {
    return slowMoveSpeed;
}

float EditorCamera::GetSmoothing() const {
    return smoothing;
}

float EditorCamera::GetZoomSpeed() const {
    return zoomSpeed;
}

EditorCamera* EditorCamera::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto editorCamParams = EditorCameraParameters();

    SetIfExists(params, "sensitivityX", editorCamParams.sensitivityX);
    SetIfExists(params, "sensitivityY", editorCamParams.sensitivityY);
    SetIfExists(params, "moveSpeed", editorCamParams.moveSpeed);
    SetIfExists(params, "fastMoveSpeed", editorCamParams.fastMoveSpeed);
    SetIfExists(params, "slowMoveSpeed", editorCamParams.slowMoveSpeed);
    SetIfExists(params, "smoothing", editorCamParams.smoothing);
    SetIfExists(params, "zoomSpeed", editorCamParams.zoomSpeed);
    SetIfExists(params, "minZoomSpeed", editorCamParams.minZoomSpeed);
    SetIfExists(params, "maxZoomSpeed", editorCamParams.maxZoomSpeed);

    return owner->AddComponent<EditorCamera>(editorCamParams);
}

nlohmann::json EditorCamera::SerializeToJson() const {
    nlohmann::json params;

    params["sensitivityX"] = sensitivityX;
    params["sensitivityY"] = sensitivityY;
    params["moveSpeed"] = moveSpeed;
    params["fastMoveSpeed"] = fastMoveSpeed;
    params["slowMoveSpeed"] = slowMoveSpeed;
    params["smoothing"] = smoothing;
    params["zoomSpeed"] = zoomSpeed;
    params["minZoomSpeed"] = minZoomSpeed;
    params["maxZoomSpeed"] = maxZoomSpeed;

    return params;
}

REGISTER_COMPONENT_FROM_JSON_WITH_UI(EditorCamera, "Editor Camera")