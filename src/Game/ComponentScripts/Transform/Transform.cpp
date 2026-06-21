#include "Transform.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../Scene/Scene.h"
#include <glm/gtc/quaternion.hpp>
#include "../../Types/Axis.h"

Transform::Transform(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 2), position{pos}, rotation{rot}, scale{scl} {

}

void Transform::ApplyRotation(glm::mat4 &matrix) const {
    matrix = glm::rotate(matrix, glm::radians(rotation.x), Axis::xAxis);
    matrix = glm::rotate(matrix, glm::radians(rotation.y), Axis::yAxis);
    matrix = glm::rotate(matrix, glm::radians(rotation.z), Axis::zAxis);
}

glm::mat4 Transform::ToModelMatrix() const {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), Axis::xAxis);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), Axis::yAxis);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), Axis::zAxis);
    modelMatrix = glm::scale(modelMatrix, scale);

    return modelMatrix;
}

glm::vec3 Transform::ToForwardVector() const {
    glm::vec3 forwardVector;
    float radiansPitch = glm::radians(rotation.x);
    float radiansYaw = glm::radians(rotation.y);
    forwardVector.x = glm::sin(radiansYaw) * glm::cos(radiansPitch);
    forwardVector.y = glm::sin(radiansPitch);
    forwardVector.z = glm::cos(radiansYaw) * glm::cos(radiansPitch);

    return forwardVector;
}

glm::vec3 Transform::ToRightVector() const {
    glm::vec3 forward = ToForwardVector();
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    glm::vec3 rightVector = glm::cross(forward, up);
    
    if(glm::length(rightVector) < 0.001f) {
        float radiansYaw = glm::radians(rotation.y);
        rightVector.x = glm::cos(radiansYaw);
        rightVector.y = 0.0f;
        rightVector.z = -glm::sin(radiansYaw);
    }
    
    return rightVector;
}

glm::vec3 Transform::GetWorldPosition() const {
    return Utils::GetTranslationFromMatrix(GetWorldModelMatrix());
}

glm::vec3 Transform::GetWorldRotation() const {
    glm::mat4 worldMatrix = GetWorldModelMatrix();
    glm::vec3 worldScale = GetWorldScale();
    glm::mat3 rotationMatrix;
    if(worldScale.x > 0.0f && worldScale.y > 0.0f && worldScale.z > 0.0f) {
        rotationMatrix[0] = glm::vec3(worldMatrix[0]) / worldScale.x;
        rotationMatrix[1] = glm::vec3(worldMatrix[1]) / worldScale.y;
        rotationMatrix[2] = glm::vec3(worldMatrix[2]) / worldScale.z;
    } else {
        rotationMatrix = glm::mat3(worldMatrix);
    }

    glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
    glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);
    return glm::degrees(eulerAngles);
}

glm::vec3 Transform::GetWorldScale() const {
    return Utils::GetScaleFromMatrix(GetWorldModelMatrix());
}

glm::mat4 Transform::GetWorldModelMatrix() const {
    auto owner = GetGameObject();
    if(!owner) {
        return ToModelMatrix();
    }

    auto parentObject = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(owner->parentName));
    if(!parentObject) {
        return ToModelMatrix();
    }

    return parentObject->GetComponent<Transform>()->GetWorldModelMatrix() * ToModelMatrix();
}

void Transform::Update() {
    worldModelMatrix = GetWorldModelMatrix();
}

void Transform::AdjustToParent() {
    auto owner = GetGameObject();
    if(!owner) {
        return;
    }

    std::string parentName = owner->GetParentName();
    if(parentName.empty() || parentName == UNDEFINED_NAME) {
        return;
    }

    auto parentObject = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(parentName));
    if(!parentObject) {
        return;
    }

    auto* parentTransform = parentObject->GetComponent<Transform>();
    if(!parentTransform) {
        return;
    }

    glm::mat4 worldMatrix = GetWorldModelMatrix();
    glm::mat4 parentWorldMatrix = parentTransform->GetWorldModelMatrix();
    glm::mat4 localMatrix = glm::inverse(parentWorldMatrix) * worldMatrix;
    position = Utils::GetTranslationFromMatrix(localMatrix);
    scale = Utils::GetScaleFromMatrix(localMatrix);
    glm::mat3 rotationMatrix;
    if(scale.x > 0.0f && scale.y > 0.0f && scale.z > 0.0f) {
        rotationMatrix[0] = glm::vec3(localMatrix[0]) / scale.x;
        rotationMatrix[1] = glm::vec3(localMatrix[1]) / scale.y;
        rotationMatrix[2] = glm::vec3(localMatrix[2]) / scale.z;
    } else {
        rotationMatrix = glm::mat3(localMatrix);
    }
    glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
    glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);
    rotation = glm::degrees(eulerAngles);
}

glm::vec3 Transform::ToUpVector() const {
    glm::vec3 rightVector = ToRightVector();
    glm::vec3 forwardVector = ToForwardVector();
    return glm::cross(rightVector, forwardVector);
}