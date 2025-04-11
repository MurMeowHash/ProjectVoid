#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../../Types/Axis.h"
#include "../../../Utils/Utils.h"
#include "../ObjectComponent.h"

struct Transform : public ObjectComponent {
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

    void Update() override;
private:
    NODISCARD glm::mat4 GetWorldModelMatrix() const;
    void ApplyRotation(glm::mat4 &matrix) const;
};