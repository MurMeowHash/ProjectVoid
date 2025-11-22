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
      isLeftMouseDown(false) {
}

void EditorMovement::Start() {
    auto* transform = GetGameObject()->GetComponent<Transform>();

    if(transform) {
        currentYaw = transform->rotation.y;
        currentPitch = transform->rotation.x;
    }
}

void EditorMovement::Update() {
    // Перевіряємо, чи курсор не на UI елементах
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

    if(Input::GetMouseButton(Input::MouseButton::MouseLeft)) {
        if(!isLeftMouseDown) {
            isLeftMouseDown = true;
        }

        float mouseX = Input::GetMouseOffsetX() * sensitivityX;
        float mouseY = Input::GetMouseOffsetY() * sensitivityY;
        currentYaw += mouseX;
        currentPitch -= mouseY;

        currentPitch = glm::clamp(currentPitch, -89.0f, 89.0f);

        transform->rotation.x = currentPitch;
        transform->rotation.y = currentYaw;
    } else {
        if(isLeftMouseDown) {
            isLeftMouseDown = false;
        }
    }
}

void EditorMovement::HandleMovement() const {
    auto* transform = GetGameObject()->GetComponent<Transform>();
    if(!transform) return;

    glm::vec3 targetVelocity = glm::vec3(0.0f);
    bool hasKeyboardInput = false;

    glm::vec3 forward = glm::normalize(transform->ToForwardVector());
    glm::vec3 right = glm::normalize(transform->ToRightVector());
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    if(Input::GetKey(Input::Key::KeyW)) {
        targetVelocity += forward;
        hasKeyboardInput = true;
    }
    if(Input::GetKey(Input::Key::KeyS)) {
        targetVelocity -= forward;
        hasKeyboardInput = true;
    }
    if(Input::GetKey(Input::Key::KeyD)) {
        targetVelocity += right;
        hasKeyboardInput = true;
    }
    if(Input::GetKey(Input::Key::KeyA)) {
        targetVelocity -= right;
        hasKeyboardInput = true;
    }

    if(Input::GetKey(Input::Key::KeyE)) {
        targetVelocity += up;
        hasKeyboardInput = true;
    }
    if(Input::GetKey(Input::Key::KeyQ)) {
        targetVelocity -= up;
        hasKeyboardInput = true;
    }

    if(!hasKeyboardInput) {
        return;
    }

    if(glm::length(targetVelocity) > 0.0f) {
        targetVelocity = glm::normalize(targetVelocity);
    }

    float currentSpeed = moveSpeed;
    if(Input::GetKey(Input::Key::KeyLeftShift)) {
        currentSpeed = fastMoveSpeed;
    } else if(Input::GetKey(Input::Key::KeyLeftControl)) {
        currentSpeed = slowMoveSpeed;
    }

    targetVelocity *= currentSpeed;

    float deltaTime = Time::GetDeltaTime();
    transform->position += targetVelocity * deltaTime;
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

REGISTER_COMPONENT_FROM_JSON_WITH_UI(EditorMovement, "Editor Movement")