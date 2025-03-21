#pragma once
#include "../CreateParameters.h"
#include "../../ComponentScripts/ObjectComponent.h"

class Camera : public ObjectComponent {
private:
    glm::vec3 worldUp;
    ProjectionMode projectionMode;
    PerspectiveParameters perspectiveParameters;
    OrthographicParameters orthographicParameters;
    float nearPlane;
    float farPlane;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    void UpdateView();
    void UpdateProjection();
public:
    explicit Camera(const CameraParameters &params = CameraParameters());
    NODISCARD glm::mat4 GetViewMatrix() const;
    NODISCARD glm::mat4 GetProjectionMatrix() const;

    void Update() override;
};