#include "InspectorPanel.h"
#include "../EditorStyles.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Game/ComponentScripts/ObjectComponent.h"
#include "../../Game/ComponentScripts/Transform/Transform.h"
#include "../../Game/ComponentScripts/Rigidbody/Rigidbody.h"
#include "../../Game/ComponentScripts/Collider/Collider.h"
#include "../../Game/ComponentScripts/ComponentRegistry.h"
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
    : EditorPanel("Inspector", pos, size), selectedObjectName(""), nameBufferDirty(false) {
    nameBuffer[0] = '\0';
}

void InspectorPanel::SetSelectedObject(const std::string& objectName) {
    if(selectedObjectName != objectName) {
        selectedObjectName = objectName;
        nameBufferDirty = true;
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

    ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorStyles::UI::CHILD_BG);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorStyles::Frame::BG);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, EditorStyles::Frame::HOVERED);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, EditorStyles::Frame::ACTIVE);
    ImGui::PushStyleColor(ImGuiCol_Button, EditorStyles::Button::DEFAULT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyles::Button::HOVERED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorStyles::Button::ACTIVE);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, EditorStyles::UI::SLIDER_GRAB);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, EditorStyles::UI::SLIDER_GRAB_ACTIVE);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, EditorStyles::UI::CHECKMARK);
    
    ImGui::BeginChild("InspectorContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    RenderObjectName(obj);
    ImGui::Spacing();

    RenderContextMenu(obj);
    RenderComponents(obj);

    ImGui::Spacing();
    ImGui::EndChild();

    ImGui::PopStyleColor(10);
    ImGui::End();
}

void InspectorPanel::RenderObjectName(GameObject* obj) {
    if(nameBufferDirty) {
        strncpy_s(nameBuffer, sizeof(nameBuffer), selectedObjectName.c_str(), _TRUNCATE);
        nameBufferDirty = false;
    }

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

        ImGui::PushStyleColor(ImGuiCol_Header, EditorStyles::Header::DEFAULT);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorStyles::Header::HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorStyles::Header::ACTIVE);
        bool headerOpen = ImGui::CollapsingHeader(componentTypeName.c_str(), 
                                                   ImGuiTreeNodeFlags_SpanFullWidth);
        ImGui::PopStyleColor(3);

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
            component->RenderUI(obj);
        }
    }

    if(!componentTypeToRemove.empty()) {
        RemoveComponentByType(obj, componentTypeToRemove);
    }
}
