#include "InspectorPanel.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Game/ComponentScripts/Transform/Transform.h"
#include "../../Game/ComponentScripts/Rigidbody/Rigidbody.h"
#include "../../Game/ComponentScripts/MeshRenderData/MeshRenderData.h"
#include "../../Game/ComponentScripts/Light/Light.h"
#include "../../Game/ComponentScripts/Camera/Camera.h"
#include "../../Game/ComponentScripts/Collider/BoxCollider.h"
#include "../../Game/ComponentScripts/Collider/CapsuleCollider.h"
#include "../../Game/ComponentScripts/Movement/Movement.h"
#include "../../Game/ComponentScripts/Movement/MouseLook.h"
#include "../../Game/ComponentScripts/ComponentRegistry.h"
#include "../../Game/Types/CreateParameters.h"
#include "../../Core/Resources/ResourceManager.h"
#include <imgui/imgui.h>
#include <typeindex>
#include <algorithm>

// Допоміжна функція для видалення компонента за його типом
static void RemoveComponentByType(GameObject* obj, const std::string& componentTypeName) {
    if(!obj || componentTypeName.empty()) return;
    
    // Використовуємо реєстр компонентів для видалення
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
    
    // Оновлюємо буфер назви, якщо об'єкт змінився
    if(nameBufferDirty) {
        strncpy_s(nameBuffer, sizeof(nameBuffer), selectedObjectName.c_str(), _TRUNCATE);
        nameBufferDirty = false;
    }
    
    // Редагування назви об'єкта
    ImGui::Text("Name:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##ObjectName", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
    
    // Оновлюємо назву об'єкта тільки після завершення редагування (Enter або втрата фокусу)
    if(ImGui::IsItemDeactivatedAfterEdit()) {
        std::string newName(nameBuffer);
        if(!newName.empty()) {
            int objectIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
            if(objectIndex != Scene::ABSENT_OBJECT) {
                // Оновлюємо назву об'єкта
                Scene::SetObjectName(static_cast<uint>(objectIndex), newName);
                // Оновлюємо selectedObjectName на нову унікальну назву
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
            // Якщо назва порожня, повертаємо початкову назву
            strncpy_s(nameBuffer, sizeof(nameBuffer), selectedObjectName.c_str(), _TRUNCATE);
        }
    }
    
    ImGui::Spacing();
    
    // Контекстне меню при правому кліку на пустому місці панелі
    if(ImGui::BeginPopupContextWindow("InspectorContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        // Підменю для додавання компонентів
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
        
        ImGui::EndPopup();
    }
    
    // Список компонентів
    auto allComponents = obj->GetAllComponents();
    if(!allComponents.empty()) {
        std::string componentTypeToRemove;
        
        for(auto* component : allComponents) {
            std::string componentTypeName = component->GetComponentTypeName();
            if(componentTypeName.empty()) {
                // Якщо компонент не має назви, використовуємо typeid
                componentTypeName = typeid(*component).name();
                // Прибираємо префікси з назви типу (якщо є)
                size_t pos = componentTypeName.find_last_of("::");
                if(pos != std::string::npos) {
                    componentTypeName = componentTypeName.substr(pos + 1);
                }
            }
            
            // Відображаємо компонент як випадаючий список
            bool isTransform = std::type_index(typeid(*component)) == std::type_index(typeid(Transform));
            ImGui::PushID(component);
            bool headerOpen = ImGui::CollapsingHeader(componentTypeName.c_str());
            
            // Контекстне меню при правому кліку на компонент
            if(ImGui::BeginPopupContextItem()) {
                // Підменю для додавання компонентів
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
                
                // Опція видалення (крім Transform)
                if(!isTransform) {
                    if(ImGui::MenuItem("Remove Component")) {
                        componentTypeToRemove = componentTypeName;
                    }
                }
                
                ImGui::EndPopup();
            }
            ImGui::PopID();
            
            // Перевіряємо, чи потрібно вийти з циклу після видалення
            if(!componentTypeToRemove.empty() && componentTypeToRemove == componentTypeName) {
                break;
            }
            
            if(headerOpen && componentTypeToRemove != componentTypeName) {
                // Спеціальна обробка для Transform компонента
                if(isTransform) {
                    auto* transform = static_cast<Transform*>(component);
                    // Перевіряємо наявність Rigidbody
                    auto* rb = obj->GetComponent<Rigidbody>();
                    
                    // Оновлюємо кешовані значення, якщо поля не активні
                    if(!positionActive) {
                        cachedPosition = transform->position;
                    }
                    if(!rotationActive) {
                        cachedRotation = transform->rotation;
                    }
                    if(!scaleActive) {
                        cachedScale = transform->scale;
                    }
                    
                    // Редагування Position
                    ImGui::Text("Position:");
                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputFloat3("##Position", &cachedPosition.x);
                    positionActive = ImGui::IsItemActive();
                    if(ImGui::IsItemDeactivatedAfterEdit()) {
                        if(rb) {
                            // Для об'єктів з Rigidbody використовуємо Move()
                            rb->Move(cachedPosition, cachedRotation);
                        } else {
                            // Для звичайних об'єктів оновлюємо напряму
                            transform->position = cachedPosition;
                        }
                    }
                    
                    // Редагування Rotation
                    ImGui::Text("Rotation:");
                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputFloat3("##Rotation", &cachedRotation.x);
                    rotationActive = ImGui::IsItemActive();
                    if(ImGui::IsItemDeactivatedAfterEdit()) {
                        if(rb) {
                            // Для об'єктів з Rigidbody використовуємо Move()
                            rb->Move(cachedPosition, cachedRotation);
                        } else {
                            // Для звичайних об'єктів оновлюємо напряму
                            transform->rotation = cachedRotation;
                        }
                    }
                    
                    // Редагування Scale
                    ImGui::Text("Scale:");
                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputFloat3("##Scale", &cachedScale.x);
                    scaleActive = ImGui::IsItemActive();
                    if(ImGui::IsItemDeactivatedAfterEdit()) {
                        // Scale не впливає на Rigidbody, тому оновлюємо напряму
                        transform->scale = cachedScale;
                    }
                }
                // Спеціальна обробка для MeshRenderData компонента
                else if(std::type_index(typeid(*component)) == std::type_index(typeid(MeshRenderData))) {
                    auto* meshRenderData = static_cast<MeshRenderData*>(component);
                    
                    // Отримуємо поточну модель (якщо є) для визначення індексу в списку
                    // Спочатку перевіряємо збережену назву моделі, потім намагаємося знайти за мешами
                    std::string currentModelName = meshRenderData->modelName;
                    if(currentModelName.empty() && !meshRenderData->meshes.empty()) {
                        currentModelName = ResourceManager::GetModelNameByMeshes(meshRenderData->meshes);
                    }
                    
                    // Випадаючий список для вибору моделі
                    ImGui::Text("Select Model:");
                    ImGui::SetNextItemWidth(-1);
                    
                    auto modelNames = ResourceManager::GetAllModelNames();
                    if(modelNames.empty()) {
                        ImGui::Text("No models loaded");
                    } else {
                        // Створюємо масив C-рядків для Combo
                        std::vector<const char*> modelNamesCStr;
                        modelNamesCStr.reserve(modelNames.size() + 1);
                        modelNamesCStr.push_back("None");
                        for(const auto& name : modelNames) {
                            modelNamesCStr.push_back(name.c_str());
                        }
                        
                        // Знаходимо індекс поточної моделі
                        int currentIndex = 0;
                        if(!currentModelName.empty()) {
                            auto it = std::find(modelNames.begin(), modelNames.end(), currentModelName);
                            if(it != modelNames.end()) {
                                currentIndex = static_cast<int>(std::distance(modelNames.begin(), it) + 1);
                            }
                        }
                        
                        // Відображаємо Combo
                        if(ImGui::Combo("##ModelSelect", &currentIndex, modelNamesCStr.data(), static_cast<int>(modelNamesCStr.size()))) {
                            if(currentIndex == 0) {
                                // Вибрано "None" - очищаємо меші
                                meshRenderData->meshes.clear();
                                meshRenderData->modelName.clear();
                            } else {
                                // Вибрано модель - замінюємо меші на меші вибраної моделі
                                std::string selectedModelName = modelNames[currentIndex - 1];
                                int modelIndex = ResourceManager::GetModelIndexByName(selectedModelName);
                                if(modelIndex != ABSENT_RESOURCE) {
                                    auto* model = ResourceManager::GetModelByIndex(modelIndex);
                                    if(model) {
                                        // Очищаємо поточні меші
                                        meshRenderData->meshes.clear();
                                        // Додаємо меші вибраної моделі
                                        Scene::AddModelMeshesToGameObject(obj, model);
                                        // Оновлюємо назву моделі
                                        meshRenderData->modelName = selectedModelName;
                                    }
                                }
                            }
                        }
                    }
                }
                // Спеціальна обробка для Light компонента
                else if(std::type_index(typeid(*component)) == std::type_index(typeid(Light))) {
                    auto* light = static_cast<Light*>(component);
                    
                    // Редагування Type
                    ImGui::Text("Type:");
                    ImGui::SetNextItemWidth(-1);
                    const char* lightTypes[] = { "Directional", "Point", "Spot" };
                    int currentType = static_cast<int>(light->type);
                    if(ImGui::Combo("##LightType", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes))) {
                        light->type = static_cast<LightType>(currentType);
                    }
                    
                    // Редагування Color
                    ImGui::Text("Color:");
                    ImGui::SetNextItemWidth(-1);
                    float colorArray[3] = { light->color.x, light->color.y, light->color.z };
                    if(ImGui::ColorEdit3("##LightColor", colorArray)) {
                        light->color = glm::vec3(colorArray[0], colorArray[1], colorArray[2]);
                    }
                    
                    // Редагування Intensity
                    ImGui::Text("Intensity:");
                    ImGui::SetNextItemWidth(-1);
                    if(ImGui::InputFloat("##LightIntensity", &light->intensity)) {
                        // Забезпечуємо, щоб інтенсивність не була від'ємною
                        if(light->intensity < 0.0f) {
                            light->intensity = 0.0f;
                        }
                    }
                    
                    // Редагування Radius для Point і Spot світла
                    if(light->type == LightType::Point || light->type == LightType::Spot) {
                        ImGui::Text("Radius:");
                        ImGui::SetNextItemWidth(-1);
                        if(ImGui::InputFloat("##LightRadius", &light->radius)) {
                            // Забезпечуємо, щоб радіус не був від'ємним
                            if(light->radius < 0.0f) {
                                light->radius = 0.0f;
                            }
                        }
                    }
                    
                    // Редагування Inner и Outer CutOff для Spot світла
                    if(light->type == LightType::Spot) {
                        // Редагування Inner CutOff
                        ImGui::Text("Inner CutOff:");
                        ImGui::SetNextItemWidth(-1);
                        if(ImGui::InputFloat("##LightInnerCutOff", &light->innerCutOff)) {
                            // Забезпечуємо, щоб innerCutOff був між 0 та outerCutOff
                            if(light->innerCutOff < 0.0f) {
                                light->innerCutOff = 0.0f;
                            }
                            if(light->innerCutOff > light->outerCutOff) {
                                light->innerCutOff = light->outerCutOff;
                            }
                        }
                        
                        // Редагування Outer CutOff
                        ImGui::Text("Outer CutOff:");
                        ImGui::SetNextItemWidth(-1);
                        if(ImGui::InputFloat("##LightOuterCutOff", &light->outerCutOff)) {
                            // Забезпечуємо, щоб outerCutOff був більше innerCutOff
                            if(light->outerCutOff < light->innerCutOff) {
                                light->outerCutOff = light->innerCutOff;
                            }
                        }
                    }
                } else {
                    // Для інших компонентів
                    ImGui::Text("Component details coming soon...");
                }
            }
        }
        
        // Видаляємо компонент після циклу, щоб уникнути проблем з ітератором
        if(!componentTypeToRemove.empty()) {
            RemoveComponentByType(obj, componentTypeToRemove);
        }
    }
    
    ImGui::Spacing();
    
    ImGui::EndChild();
    
    ImGui::End();
}

