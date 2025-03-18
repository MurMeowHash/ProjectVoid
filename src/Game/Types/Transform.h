#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Axis.h"
#include "../../Utils/Utils.h"

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    NODISCARD glm::mat4 ToModelMatrix() const {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), Axis::xAxis);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), Axis::yAxis);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), Axis::zAxis);
        modelMatrix = glm::scale(modelMatrix, scale);

        return modelMatrix;
    }
};