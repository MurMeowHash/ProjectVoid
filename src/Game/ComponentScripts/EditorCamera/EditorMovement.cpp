#include "EditorMovement.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include "../../../Engine/Time/Time.h"
#include "../Transform/Transform.h"
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

EditorMovement::EditorMovement(const EditorMovementParameters& params)
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
      isRightMouseDown(false) {
}

void EditorMovement::Start() {
    auto* transform = GetGameObject()->GetComponent<Transform>();

    if(transform) {
        currentYaw = transform->rotation.y;
        currentPitch = transform->rotation.x;
    }
}

void EditorMovement::Update() {
    ImGuiIO& io = ImGui::GetIO();
    if(io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return;
    }
    
    HandleCameraRotation();
    HandleMovement();
    HandleZoom();
}

void EditorMovement::HandleCameraRotation() {
    auto* transform = GetGameObject()->GetComponent<Transform>();
    if(!transform) return;

    if(Input::GetMouseButton(Input::MouseButton::MouseRight)) {
        if(!isRightMouseDown) {
            isRightMouseDown = true;
        }

        float mouseX = Input::GetMouseOffsetX() * sensitivityX;
        float mouseY = Input::GetMouseOffsetY() * sensitivityY;
        currentYaw -= mouseX;
        currentPitch += mouseY;

        currentPitch = glm::clamp(currentPitch, -89.0f, 89.0f);

        transform->rotation.x = currentPitch;
        transform->rotation.y = currentYaw;
    } else {
        if(isRightMouseDown) {
            isRightMouseDown = false;
        }
    }
}

void EditorMovement::HandleMovement() const {
    Transform* transform = GetGameObject()->GetComponent<Transform>();
    glm::vec3 movementDirection = GetMovementDirection();
    transform->position += movementDirection * moveSpeed * Time::GetDeltaTime();
}

void EditorMovement::HandleZoom() {
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

void EditorMovement::SetSensitivityX(float sens) {
    sensitivityX = sens;
}

void EditorMovement::SetSensitivityY(float sens) {
    sensitivityY = sens;
}

void EditorMovement::SetMoveSpeed(float speed) {
    moveSpeed = speed;
}

void EditorMovement::SetFastMoveSpeed(float speed) {
    fastMoveSpeed = speed;
}

void EditorMovement::SetSlowMoveSpeed(float speed) {
    slowMoveSpeed = speed;
}

void EditorMovement::SetSmoothing(float smooth) {
    smoothing = smooth;
}

void EditorMovement::SetZoomSpeed(float speed) {
    zoomSpeed = speed;
}

float EditorMovement::GetSensitivityX() const {
    return sensitivityX;
}

float EditorMovement::GetSensitivityY() const {
    return sensitivityY;
}

float EditorMovement::GetMoveSpeed() const {
    return moveSpeed;
}

float EditorMovement::GetFastMoveSpeed() const {
    return fastMoveSpeed;
}

float EditorMovement::GetSlowMoveSpeed() const {
    return slowMoveSpeed;
}

float EditorMovement::GetSmoothing() const {
    return smoothing;
}

float EditorMovement::GetZoomSpeed() const {
    return zoomSpeed;
}

EditorMovement* EditorMovement::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto editorCamParams = EditorMovementParameters();

    SetIfExists(params, "sensitivityX", editorCamParams.sensitivityX);
    SetIfExists(params, "sensitivityY", editorCamParams.sensitivityY);
    SetIfExists(params, "moveSpeed", editorCamParams.moveSpeed);
    SetIfExists(params, "fastMoveSpeed", editorCamParams.fastMoveSpeed);
    SetIfExists(params, "slowMoveSpeed", editorCamParams.slowMoveSpeed);
    SetIfExists(params, "smoothing", editorCamParams.smoothing);
    SetIfExists(params, "zoomSpeed", editorCamParams.zoomSpeed);
    SetIfExists(params, "minZoomSpeed", editorCamParams.minZoomSpeed);
    SetIfExists(params, "maxZoomSpeed", editorCamParams.maxZoomSpeed);

    return owner->AddComponent<EditorMovement>(editorCamParams);
}

nlohmann::json EditorMovement::SerializeToJson() const {
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

glm::vec3 EditorMovement::GetMovementDirection() const {
    glm::vec3 movementDirection = glm::vec3(0.0f);
    movementDirection += GetKeysMovementDirection();
    movementDirection += GetMouseWheelMovementDirection();
    return glm::length(movementDirection) == 0.0f ? movementDirection : glm::normalize(movementDirection);
}

glm::vec3 EditorMovement::GetMouseWheelMovementDirection() const {
    if(!Input::GetMouseButton(Input::MouseButton::MouseWheel))
        return glm::vec3(0.0f);

    float mouseOffsetX = Input::GetMouseOffsetX();
    float mouseOffsetY = Input::GetMouseOffsetY();
    glm::vec3 movementDirection = glm::vec3(0.0f);
    movementDirection -= GetGameObject()->GetComponent<Transform>()->ToRightVector() * mouseOffsetX;
    movementDirection += GetGameObject()->GetComponent<Transform>()->ToUpVector() * -mouseOffsetY;
    return movementDirection;
}

glm::vec3 EditorMovement::GetKeysMovementDirection() const {
    if(!Input::GetMouseButton(Input::MouseButton::MouseRight))
        return glm::vec3(0.0f);

    Transform* transform = GetGameObject()->GetComponent<Transform>();
    glm::vec3 movementDirection = glm::vec3(0.0f);
    if(Input::GetKey(Input::Key::KeyW))
        movementDirection += transform->ToForwardVector();
    if(Input::GetKey(Input::Key::KeyS))
        movementDirection -= transform->ToForwardVector();
    if(Input::GetKey(Input::Key::KeyD))
        movementDirection += transform->ToRightVector();
    if(Input::GetKey(Input::Key::KeyA))
        movementDirection -= transform->ToRightVector();

    return movementDirection;
}

REGISTER_COMPONENT_FROM_JSON_WITH_UI(EditorMovement, "Editor Movement")