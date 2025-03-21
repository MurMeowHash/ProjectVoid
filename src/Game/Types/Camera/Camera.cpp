#include "Camera.h"
#include "../../../Debug/Debug.h"
#include "../GameObject/GameObject.h"
#include "../../Scene/Scene.h"

Camera::Camera(const CameraParameters &params)
: ObjectComponent(2), worldUp{params.worldUp}, projectionMode{params.projectionMode},
  perspectiveParameters{params.perspectiveParameters}, orthographicParameters{params.orthographicParameters},
  nearPlane{params.nearPlane}, farPlane{params.farPlane} {

}

void Camera::UpdateView() {
    auto owner = GetGameObject();
    if(!owner) return;

    auto ownerTransform = owner->GetComponent<Transform>();
    auto forwardDirection = glm::normalize(ownerTransform->ToForwardVector());
    viewMatrix = glm::lookAt(ownerTransform->position, ownerTransform->position + forwardDirection, worldUp);
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

void Camera::Update() {
    UpdateView();
    UpdateProjection();
}