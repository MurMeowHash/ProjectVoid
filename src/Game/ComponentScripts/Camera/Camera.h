#pragma once
#include "../../Types/CreateParameters.h"
#include "../ObjectComponent.h"
#include "../ComponentMacros.h"
#include "../../Types/GLBuffer/FrameBuffer/FrameBuffer.h"
#include "../../../Engine/Physics/Ray.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class Camera : public ObjectComponent {
DEFINE_BASE(void)
private:
    glm::vec3 worldUp;
    ProjectionMode projectionMode;
    PerspectiveParameters perspectiveParameters;
    OrthographicParameters orthographicParameters;
    float nearPlane;
    float farPlane;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    uint cameraPriority;
    int renderTarget;

    void UpdateView();
    void UpdateProjection();
public:
    explicit Camera(const CameraParameters &params = CameraParameters());
    NODISCARD glm::mat4 GetViewMatrix() const;
    NODISCARD glm::mat4 GetProjectionMatrix() const;
    NODISCARD uint GetCameraPriority() const;
    NODISCARD int GetRenderTarget() const;
    NODISCARD Ray GetRayFromScreenPoint(int scrX, int scrY) const;

    void SetRenderTexture(int textureIndex);
    void SetCameraPriority(uint priorityValue);

    void Update() override;
    void RenderUI(GameObject* obj) override;
    
    NODISCARD nlohmann::json SerializeToJson() const override;
    static Camera* CreateFromJson(GameObject* owner, const nlohmann::json& params);
    GET_COMPONENT_TYPE_NAME(Camera)
};