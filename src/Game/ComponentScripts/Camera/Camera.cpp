#include "Camera.h"
#include "../../../Debug/Debug.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Renderer/Renderer.h"
#include "../../../Core/Core.h"

Camera::Camera(const CameraParameters &params)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 3), worldUp{params.worldUp}, projectionMode{params.projectionMode},
  perspectiveParameters{params.perspectiveParameters}, orthographicParameters{params.orthographicParameters},
  nearPlane{params.nearPlane}, farPlane{params.farPlane}, renderTarget{params.renderTarget}, cameraPriority{params.cameraPriority} {

}

void Camera::UpdateView() {
    auto owner = GetGameObject();
    if(!owner) return;

    auto ownerTransform = owner->GetComponent<Transform>();
    auto forwardDirection = glm::normalize(ownerTransform->ToForwardVector());
    auto worldPos = ownerTransform->GetWorldPosition();
    viewMatrix = glm::lookAt(worldPos, worldPos + forwardDirection, worldUp);
}

void Camera::UpdateProjection() {
    if(projectionMode == ProjectionMode::Perspective) {
        projectionMatrix = glm::perspective(glm::radians(perspectiveParameters.fov), perspectiveParameters.aspectRatio,
                                            nearPlane, farPlane);
    }
    else if(projectionMode == ProjectionMode::Orthographic) {
        projectionMatrix = glm::ortho(orthographicParameters.left, orthographicParameters.right, orthographicParameters.bottom,
                                      orthographicParameters.top, nearPlane, farPlane);
    }
    else {
        Debug::LogError("Camera", "Unknown projection mode", static_cast<uint>(projectionMode));
    }
}

glm::mat4 Camera::GetViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return projectionMatrix;
}

uint Camera::GetCameraPriority() const {
    return cameraPriority;
}

int Camera::GetRenderTarget() const {
    return renderTarget;
}

Ray Camera::GetRayFromScreenPoint(int scrX, int scrY) const {
    auto ndcX = 2.0f * static_cast<float>(scrX) / static_cast<float>(Core::GetScreenWidth()) - 1.0f;
    auto ndcY = 1.0f - 2.0f * static_cast<float>(scrY) / static_cast<float>(Core::GetScreenHeight());
    auto ndcZ = 1.0f;

    glm::vec4 rayDirection = glm::vec4(ndcX, ndcY, ndcZ, 1.0f);
    rayDirection = glm::inverse(projectionMatrix) * rayDirection;
    rayDirection.w = 0.0f;
    rayDirection = glm::inverse(viewMatrix) * rayDirection;
    auto normalizedRayDir = glm::normalize(glm::vec3(rayDirection));
    auto owner = GetGameObject();
    if(!owner) return Ray{glm::vec3(0.0f), normalizedRayDir};

    glm::vec3 origin = owner->GetComponent<Transform>()->GetWorldPosition();
    return Ray{origin, normalizedRayDir};
}

void Camera::Update() {
    UpdateView();
    UpdateProjection();
}

void Camera::SetRenderTexture(int textureIndex) {
    if(textureIndex == ABSENT_RESOURCE) {
        renderTarget = DEFAULT_RENDER_TARGET;
        return;
    }

    renderTarget = Renderer::CreateUserFrameBuffer(textureIndex);
}

void Camera::SetCameraPriority(uint priorityValue) {
    cameraPriority = priorityValue;
}