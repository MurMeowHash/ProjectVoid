#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../../Types/Axis.h"
#include "../../../Utils/Utils.h"
#include "../ObjectComponent.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

struct Transform : public ObjectComponent {
DEFINE_BASE(void)

    static constexpr glm::vec3 DEFAULT_POSITION = glm::vec3(0.0f);
    static constexpr glm::vec3 DEFAULT_ROTATION = glm::vec3(0.0f);
    static constexpr glm::vec3 DEFAULT_SCALE = glm::vec3(1.0f);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 worldModelMatrix = glm::mat4(1.0f);

    explicit Transform(const glm::vec3 &pos = DEFAULT_POSITION, const glm::vec3 &rot = DEFAULT_ROTATION,
                       const glm::vec3 &scl = DEFAULT_SCALE);

    NODISCARD glm::mat4 ToModelMatrix() const;
    NODISCARD glm::vec3 ToForwardVector() const;
    NODISCARD glm::vec3 ToRightVector() const;
    NODISCARD glm::vec3 GetWorldPosition() const;
    NODISCARD glm::vec3 GetWorldRotation() const;
    NODISCARD glm::vec3 GetWorldScale() const;

    void Update() override;
    void AdjustToParent();
    void RenderUI(GameObject* obj) override;

    static Transform* CreateFromJson(GameObject* owner, const nlohmann::json& params);
    NODISCARD nlohmann::json SerializeToJson() const override;
    GET_COMPONENT_TYPE_NAME(Transform)
private:
    NODISCARD glm::mat4 GetWorldModelMatrix() const;
    void ApplyRotation(glm::mat4 &matrix) const;
    
    // Кеш для UI редагування
    glm::vec3 cachedPosition;
    glm::vec3 cachedRotation;
    glm::vec3 cachedScale;
    bool positionActive = false;
    bool rotationActive = false;
    bool scaleActive = false;
    
    void UpdateCache();
};