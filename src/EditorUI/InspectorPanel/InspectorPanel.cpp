#include "InspectorPanel.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Game/ComponentScripts/ObjectComponent.h"
#include "../../Game/ComponentScripts/Transform/Transform.h"
#include "../../Game/ComponentScripts/Rigidbody/Rigidbody.h"
#include "../../Game/ComponentScripts/Collider/Collider.h"
#include "../../Game/ComponentScripts/MeshRenderData/MeshRenderData.h"
#include "../../Game/ComponentScripts/Light/Light.h"
#include "../../Game/ComponentScripts/Camera/Camera.h"
#include "../../Game/ComponentScripts/ComponentRegistry.h"
#include "../../Core/Resources/ResourceManager.h"
#include <typeindex>
#include <algorithm>

static void RemoveComponentByType(GameObject* obj, const std::string& componentTypeName) {
    if(!obj || componentTypeName.empty()) return;

    const auto& uiComponents = ComponentRegistry::Instance().GetUIComponents();
    auto it = uiComponents.find(componentTypeName);
    if(it != uiComponents.end() && it->second.removeFunc) {
        it->second.removeFunc(obj);
    }
}

InspectorPanel::InspectorPanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("Inspector", pos, size), selectedObjectName(""), nameBufferDirty(false),
      cachedPosition(0.0f), cachedRotation(0.0f), cachedScale(1.0f),
      positionActive(false), rotationActive(false), scaleActive(false) {
    nameBuffer[0] = '\0';
}

void InspectorPanel::SetSelectedObject(const std::string& objectName) {
    if(selectedObjectName != objectName) {
        selectedObjectName = objectName;
        nameBufferDirty = true;
        positionActive = false;
        rotationActive = false;
        scaleActive = false;
    }
}

void InspectorPanel::Render() {
    RenderDefaultPanel();
    ImGui::SetCursorPosY(38.0f);

    if(selectedObjectName.empty()) {
        ImGui::Text("No object selected");
        ImGui::End();
        return;
    }

    if(!Scene::IsInitialized()) {
        ImGui::Text("Scene not initialized");
        ImGui::End();
        return;
    }

    auto* obj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(selectedObjectName));
    if(!obj) {
        ImGui::Text("Object not found");
        ImGui::End();
        return;
    }

    ImGui::BeginChild("InspectorContent", ImVec2(0, 0), false);

    RenderObjectName(obj);
    ImGui::Spacing();

    RenderContextMenu(obj);
    RenderComponents(obj);

    ImGui::Spacing();
    ImGui::EndChild();
    ImGui::End();
}

void InspectorPanel::RenderObjectName(GameObject* obj) {
    if(nameBufferDirty) {
        strncpy_s(nameBuffer, sizeof(nameBuffer), selectedObjectName.c_str(), _TRUNCATE);
        nameBufferDirty = false;
    }

    ImGui::Text("Name:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##ObjectName", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

    if(ImGui::IsItemDeactivatedAfterEdit()) {
        std::string newName(nameBuffer);
        if(!newName.empty()) {
            int objectIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
            if(objectIndex != Scene::ABSENT_OBJECT) {
                Scene::SetObjectName(static_cast<uint>(objectIndex), newName);
                auto* updatedObj = Scene::GetGameObjectByIndex(objectIndex);
                if(updatedObj) {
                    std::string updatedName = updatedObj->GetName();
                    if(updatedName != selectedObjectName) {
                        selectedObjectName = updatedName;
                        strncpy_s(nameBuffer, sizeof(nameBuffer), selectedObjectName.c_str(), _TRUNCATE);
                    }
                }
            }
        } else {
            strncpy_s(nameBuffer, sizeof(nameBuffer), selectedObjectName.c_str(), _TRUNCATE);
        }
    }
}

void InspectorPanel::RenderContextMenu(GameObject* obj) {
    if(ImGui::BeginPopupContextWindow("InspectorContextMenu",
        ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        RenderAddComponentMenu(obj);
        ImGui::EndPopup();
    }
}

void InspectorPanel::RenderAddComponentMenu(GameObject* obj) {
    if(ImGui::BeginMenu("Add Component")) {
        const auto& uiComponents = ComponentRegistry::Instance().GetUIComponents();
        for(const auto& [typeName, uiInfo] : uiComponents) {
            if(!uiInfo.hasComponentFunc(obj)) {
                if(ImGui::MenuItem(uiInfo.displayName.c_str())) {
                    uiInfo.createFunc(obj);
                }
            }
        }
        ImGui::EndMenu();
    }
}

void InspectorPanel::RenderComponents(GameObject* obj) {
    auto allComponents = obj->GetAllComponents();
    if(allComponents.empty()) return;

    std::string componentTypeToRemove;

    for(auto* component : allComponents) {
        std::string componentTypeName = component->GetComponentTypeName();
        if(componentTypeName.empty()) {
            componentTypeName = typeid(*component).name();
            size_t pos = componentTypeName.find_last_of("::");
            if(pos != std::string::npos) {
                componentTypeName = componentTypeName.substr(pos + 1);
            }
        }

        bool isTransform = std::type_index(typeid(*component)) == std::type_index(typeid(Transform));
        ImGui::PushID(component);
        bool headerOpen = ImGui::CollapsingHeader(componentTypeName.c_str());

        if(ImGui::BeginPopupContextItem()) {
            RenderAddComponentMenu(obj);

            if(!isTransform) {
                if(ImGui::MenuItem("Remove Component")) {
                    componentTypeToRemove = componentTypeName;
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();

        if(!componentTypeToRemove.empty() && componentTypeToRemove == componentTypeName) {
            break;
        }

        if(headerOpen && componentTypeToRemove != componentTypeName) {
            std::type_index typeIndex(typeid(*component));

            if(typeIndex == std::type_index(typeid(Transform))) {
                RenderTransformComponent(component, obj);
            }
            else if(typeIndex == std::type_index(typeid(MeshRenderData))) {
                RenderMeshRenderDataComponent(component, obj);
            }
            else if(typeIndex == std::type_index(typeid(Light))) {
                RenderLightComponent(component);
            }
            else if(typeIndex == std::type_index(typeid(Rigidbody))) {
                RenderRigidbodyComponent(component);
            }
            else if(dynamic_cast<Collider*>(component)) {
                RenderColliderComponent(component);
            }
            else {
                ImGui::Text("Component details coming soon...");
            }
        }
    }

    if(!componentTypeToRemove.empty()) {
        RemoveComponentByType(obj, componentTypeToRemove);
    }
}

void InspectorPanel::UpdateTransformCache(ObjectComponent* component) {
    auto* transform = static_cast<Transform*>(component);
    if(!positionActive) cachedPosition = transform->position;
    if(!rotationActive) cachedRotation = transform->rotation;
    if(!scaleActive) cachedScale = transform->scale;
}

void InspectorPanel::RenderTransformComponent(ObjectComponent* component, GameObject* obj) {
    auto* transform = static_cast<Transform*>(component);
    auto* rb = obj->GetComponent<Rigidbody>();

    UpdateTransformCache(component);

    ImGui::Text("Position:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputFloat3("##Position", &cachedPosition.x);
    positionActive = ImGui::IsItemActive();
    if(ImGui::IsItemDeactivatedAfterEdit()) {
        if(rb && rb->IsEnabled()) {
            rb->Move(cachedPosition, cachedRotation);
        } else {
            transform->position = cachedPosition;
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
            transform->rotation = cachedRotation;
        }
    }

    ImGui::Text("Scale:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputFloat3("##Scale", &cachedScale.x);
    scaleActive = ImGui::IsItemActive();
    if(ImGui::IsItemDeactivatedAfterEdit()) {
        transform->scale = cachedScale;
    }
}

void InspectorPanel::RenderMeshRenderDataComponent(ObjectComponent* component, GameObject* obj) {
    auto* meshRenderData = static_cast<MeshRenderData*>(component);

    std::string currentModelName = meshRenderData->modelName;
    if(currentModelName.empty() && !meshRenderData->meshes.empty()) {
        currentModelName = ResourceManager::GetModelNameByMeshes(meshRenderData->meshes);
    }

    ImGui::Text("Select Model:");
    ImGui::SetNextItemWidth(-1);

    auto modelNames = ResourceManager::GetAllModelNames();
    if(modelNames.empty()) {
        ImGui::Text("No models loaded");
        return;
    }

    std::vector<const char*> modelNamesCStr;
    modelNamesCStr.reserve(modelNames.size() + 1);
    modelNamesCStr.push_back("None");
    for(const auto& name : modelNames) {
        modelNamesCStr.push_back(name.c_str());
    }

    int currentIndex = 0;
    if(!currentModelName.empty()) {
        auto it = std::find(modelNames.begin(), modelNames.end(), currentModelName);
        if(it != modelNames.end()) {
            currentIndex = static_cast<int>(std::distance(modelNames.begin(), it) + 1);
        }
    }

    if(ImGui::Combo("##ModelSelect", &currentIndex, modelNamesCStr.data(),
        static_cast<int>(modelNamesCStr.size()))) {
        if(currentIndex == 0) {
            meshRenderData->meshes.clear();
            meshRenderData->modelName.clear();
        } else {
            std::string selectedModelName = modelNames[currentIndex - 1];
            int modelIndex = ResourceManager::GetModelIndexByName(selectedModelName);
            if(modelIndex != ABSENT_RESOURCE) {
                auto* model = ResourceManager::GetModelByIndex(modelIndex);
                if(model) {
                    meshRenderData->meshes.clear();
                    Scene::AddModelMeshesToGameObject(obj, model);
                    meshRenderData->modelName = selectedModelName;
                }
            }
        }
    }
}

void InspectorPanel::RenderLightComponent(ObjectComponent* component) {
    auto* light = static_cast<Light*>(component);

    ImGui::Text("Type:");
    ImGui::SetNextItemWidth(-1);
    const char* lightTypes[] = { "Directional", "Point", "Spot" };
    int currentType = static_cast<int>(light->type);
    if(ImGui::Combo("##LightType", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes))) {
        light->type = static_cast<LightType>(currentType);
    }

    ImGui::Text("Color:");
    ImGui::SetNextItemWidth(-1);
    float colorArray[3] = { light->color.x, light->color.y, light->color.z };
    if(ImGui::ColorEdit3("##LightColor", colorArray)) {
        light->color = glm::vec3(colorArray[0], colorArray[1], colorArray[2]);
    }

    ImGui::Text("Intensity:");
    ImGui::SetNextItemWidth(-1);
    if(ImGui::InputFloat("##LightIntensity", &light->intensity)) {
        if(light->intensity < 0.0f) light->intensity = 0.0f;
    }

    if(light->type == LightType::Point || light->type == LightType::Spot) {
        ImGui::Text("Radius:");
        ImGui::SetNextItemWidth(-1);
        if(ImGui::InputFloat("##LightRadius", &light->radius)) {
            if(light->radius < 0.0f) light->radius = 0.0f;
        }
    }

    if(light->type == LightType::Spot) {
        ImGui::Text("Inner CutOff:");
        ImGui::SetNextItemWidth(-1);
        if(ImGui::InputFloat("##LightInnerCutOff", &light->innerCutOff)) {
            if(light->innerCutOff < 0.0f) light->innerCutOff = 0.0f;
            if(light->innerCutOff > light->outerCutOff) light->innerCutOff = light->outerCutOff;
        }

        ImGui::Text("Outer CutOff:");
        ImGui::SetNextItemWidth(-1);
        if(ImGui::InputFloat("##LightOuterCutOff", &light->outerCutOff)) {
            if(light->outerCutOff < light->innerCutOff) light->outerCutOff = light->innerCutOff;
        }
    }
}

void InspectorPanel::RenderRigidbodyComponent(ObjectComponent* component) {
    auto* rigidbody = static_cast<Rigidbody*>(component);
    bool enabled = rigidbody->IsEnabled();
    if(ImGui::Checkbox("Enabled##Rigidbody", &enabled)) {
        rigidbody->SetEnabled(enabled);
    }
}

void InspectorPanel::RenderColliderComponent(ObjectComponent* component) {
    auto* collider = static_cast<Collider*>(component);
    bool enabled = collider->IsEnabled();
    if(ImGui::Checkbox("Enabled##Collider", &enabled)) {
        collider->SetEnabled(enabled);
    }
}
