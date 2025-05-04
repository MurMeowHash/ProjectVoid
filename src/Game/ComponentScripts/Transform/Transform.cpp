#include "Transform.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../Scene/Scene.h"
#include <glm/gtc/quaternion.hpp>

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

//TODO: suspicious approximation
glm::vec3 Transform::ToRightVector() const {
    glm::vec3 rightVector;
    float radiansYaw = glm::radians(90.0f - rotation.y);
    float radiansRoll = glm::radians(rotation.z);
    rightVector.x = -glm::sin(radiansYaw) * glm::cos(radiansRoll);
    rightVector.y = glm::sin(radiansRoll);
    rightVector.z = glm::cos(radiansYaw) * glm::cos(radiansRoll);

    return rightVector;
}

glm::vec3 Transform::GetWorldPosition() const {
    return Utils::GetTranslationFromMatrix(GetWorldModelMatrix());
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