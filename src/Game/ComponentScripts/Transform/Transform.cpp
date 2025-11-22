#include "Transform.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../Scene/Scene.h"
#include <glm/gtc/quaternion.hpp>
#include "../../../Utils/JsonUtils.h"
#include "../ComponentMacros.h"
#include "../Rigidbody/Rigidbody.h"
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

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
    
    // Обчислюємо правий вектор як перпендикулярний до forward та up
    glm::vec3 rightVector = glm::cross(forward, up);
    
    // Якщо forward паралельний up (вертикально вгору/вниз), використовуємо альтернативний метод
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
    // Витягуємо rotation зі світової матриці
    glm::mat4 worldMatrix = GetWorldModelMatrix();
    glm::vec3 worldScale = GetWorldScale();
    
    // Видаляємо scale зі світової матриці для отримання чистої rotation матриці
    glm::mat3 rotationMatrix;
    if(worldScale.x > 0.0f && worldScale.y > 0.0f && worldScale.z > 0.0f) {
        rotationMatrix[0] = glm::vec3(worldMatrix[0]) / worldScale.x;
        rotationMatrix[1] = glm::vec3(worldMatrix[1]) / worldScale.y;
        rotationMatrix[2] = glm::vec3(worldMatrix[2]) / worldScale.z;
    } else {
        rotationMatrix = glm::mat3(worldMatrix);
    }
    
    // Конвертуємо rotation матрицю в кватерніон, а потім в Euler angles
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

    // Зберігаємо поточну світову матрицю
    glm::mat4 worldMatrix = GetWorldModelMatrix();
    
    // Отримуємо батьківську світову матрицю
    glm::mat4 parentWorldMatrix = parentTransform->GetWorldModelMatrix();
    
    // Обчислюємо локальну матрицю: LocalMatrix = inverse(ParentWorldMatrix) * WorldMatrix
    glm::mat4 localMatrix = glm::inverse(parentWorldMatrix) * worldMatrix;
    
    // Витягуємо position, rotation, scale з локальної матриці
    position = Utils::GetTranslationFromMatrix(localMatrix);
    scale = Utils::GetScaleFromMatrix(localMatrix);
    
    // Витягуємо rotation з матриці (видаляємо scale для отримання чистої rotation матриці)
    glm::mat3 rotationMatrix;
    if(scale.x > 0.0f && scale.y > 0.0f && scale.z > 0.0f) {
        rotationMatrix[0] = glm::vec3(localMatrix[0]) / scale.x;
        rotationMatrix[1] = glm::vec3(localMatrix[1]) / scale.y;
        rotationMatrix[2] = glm::vec3(localMatrix[2]) / scale.z;
    } else {
        rotationMatrix = glm::mat3(localMatrix);
    }
    
    // Конвертуємо rotation матрицю в кватерніон, а потім в Euler angles
    glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
    glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);
    rotation = glm::degrees(eulerAngles);
}

Transform* Transform::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    glm::vec3 pos = DEFAULT_POSITION;
    glm::vec3 rot = DEFAULT_ROTATION;
    glm::vec3 scl = DEFAULT_SCALE;
    
    SetIfExists(params, "position", pos);
    SetIfExists(params, "rotation", rot);
    SetIfExists(params, "scale", scl);

    auto* existingTransform = owner->GetComponent<Transform>();
    if(existingTransform) {
        existingTransform->position = pos;
        existingTransform->rotation = rot;
        existingTransform->scale = scl;
        return existingTransform;
    }
    
    return owner->AddComponent<Transform>(pos, rot, scl);
}

nlohmann::json Transform::SerializeToJson() const {
    nlohmann::json params;
    params["position"] = nlohmann::json::array({position.x, position.y, position.z});
    params["rotation"] = nlohmann::json::array({rotation.x, rotation.y, rotation.z});
    params["scale"] = nlohmann::json::array({scale.x, scale.y, scale.z});
    return params;
}

void Transform::UpdateCache() {
    if(!positionActive) cachedPosition = position;
    if(!rotationActive) cachedRotation = rotation;
    if(!scaleActive) cachedScale = scale;
}

void Transform::RenderUI(GameObject* obj) {
    auto* rb = obj->GetComponent<Rigidbody>();
    
    UpdateCache();
    
    ImGui::Text("Position:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputFloat3("##Position", &cachedPosition.x);
    positionActive = ImGui::IsItemActive();
    if(ImGui::IsItemDeactivatedAfterEdit()) {
        if(rb && rb->IsEnabled()) {
            rb->Move(cachedPosition, cachedRotation);
        } else {
            position = cachedPosition;
        }
    }
    
    ImGui::Text("Rotation:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputFloat3("##Rotation", &cachedRotation.x);
    rotationActive = ImGui::IsItemActive();
    if(ImGui::IsItemDeactivatedAfterEdit()) {
        if(rb && rb->IsEnabled()) {
            rb->Move(cachedPosition, cachedRotation);
        } else {
            rotation = cachedRotation;
        }
    }
    
    ImGui::Text("Scale:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputFloat3("##Scale", &cachedScale.x);
    scaleActive = ImGui::IsItemActive();
    if(ImGui::IsItemDeactivatedAfterEdit()) {
        scale = cachedScale;
    }
    
    ImGui::Spacing();
}

glm::vec3 Transform::ToUpVector() const {
    glm::vec3 rightVector = ToRightVector();
    glm::vec3 forwardVector = ToForwardVector();
    return glm::cross(rightVector, forwardVector);
}

REGISTER_COMPONENT_FROM_JSON_WITH_UI_NO_REMOVE(Transform, "Transform")