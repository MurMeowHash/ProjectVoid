#include "ScenePanel.h"
#include "SceneHierarchyValidator.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Game/ComponentScripts/ComponentRegistry.h"
#include "../../Utils/Utils.h"
#include <functional>

ScenePanel::ScenePanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("Scene", pos, size), selectedObjectName("") {
}

std::vector<ScenePanel::GameObjectNode> ScenePanel::BuildHierarchy() {
    std::vector<GameObjectNode> rootNodes;
    std::vector<std::pair<GameObjectNode, std::string>> nodeParentPairs;

    for(int i = 0; i <= static_cast<int>(Scene::GetLastGameObjectIndex()); ++i) {
        auto* obj = Scene::GetGameObjectByIndex(i);
        if(!obj) continue;

        GameObjectNode node;
        node.name = obj->GetName();
        node.index = i;

        std::string parentName = obj->GetParentName();
        if(parentName.empty() || parentName == UNDEFINED_NAME) {
            rootNodes.push_back(node);
        } else {
            nodeParentPairs.push_back({node, parentName});
        }
    }

    for(const auto& [node, parentName] : nodeParentPairs) {
        std::function<bool(std::vector<GameObjectNode>&)> addChild = [&](std::vector<GameObjectNode>& nodes) -> bool {
            for(auto& n : nodes) {
                if(n.name == parentName) {
                    n.children.push_back(node);
                    return true;
                }
                if(addChild(n.children)) {
                    return true;
                }
            }
            return false;
        };

        if(!addChild(rootNodes)) {
            rootNodes.push_back(node);
        }
    }

    return rootNodes;
}

void ScenePanel::RenderAddComponentMenu(int objectIndex) {
    auto* obj = Scene::GetGameObjectByIndex(objectIndex);
    if(!obj) return;

    const auto& uiComponents = ComponentRegistry::Instance().GetUIComponents();
    for(const auto& [typeName, uiInfo] : uiComponents) {
        if(!uiInfo.hasComponentFunc(obj)) {
            if(ImGui::MenuItem(uiInfo.displayName.c_str())) {
                uiInfo.createFunc(obj);
            }
        }
    }
}

void ScenePanel::RenderSetParentMenu(const GameObjectNode& node) {
    auto* obj = Scene::GetGameObjectByIndex(node.index);
    if(!obj) return;

    bool isRoot = obj->GetParentName().empty() || obj->GetParentName() == UNDEFINED_NAME;
    if(ImGui::MenuItem("None", nullptr, isRoot)) {
        obj->SetParentName(UNDEFINED_NAME);
    }

    ImGui::Separator();

    for(int i = 0; i <= static_cast<int>(Scene::GetLastGameObjectIndex()); ++i) {
        auto* potentialParent = Scene::GetGameObjectByIndex(i);
        if(!potentialParent || potentialParent == obj) continue;

        std::string parentName = potentialParent->GetName();

        if(!SceneHierarchyValidator::CanSetParent(node.name, parentName)) continue;

        bool isCurrentParent = obj->GetParentName() == parentName;

        if(ImGui::MenuItem(parentName.c_str(), nullptr, isCurrentParent)) {
            obj->SetParentName(parentName);
        }
    }
}

void ScenePanel::RenderContextMenu(const GameObjectNode& node) {
    if(!ImGui::BeginPopupContextItem()) return;

    if(ImGui::MenuItem("Create GameObject")) {
        GameObjectParameters params;
        params.name = "GameObject";
        params.parentName = node.name;
        uint newObjectIndex = Scene::CreateGameObject(params);
        auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
        if(newObj) {
            SelectObject(newObj->GetName());
        }
    }

    if(ImGui::BeginMenu("Add Component")) {
        RenderAddComponentMenu(node.index);
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Set Parent")) {
        RenderSetParentMenu(node);
        ImGui::EndMenu();
    }

    if(ImGui::MenuItem("Delete")) {
        SelectObject(node.name);
        DeleteSelectedObject();
    }

    ImGui::EndPopup();
}

void ScenePanel::HandleDragDrop(const GameObjectNode& node) {
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("GAMEOBJECT_NODE", node.name.c_str(), node.name.size() + 1);
        ImGui::Text("Move %s", node.name.c_str());
        ImGui::EndDragDropSource();
    }

    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_NODE")) {
            std::string draggedName(static_cast<const char*>(payload->Data));

            if(SceneHierarchyValidator::CanSetParent(draggedName, node.name)) {
                auto* draggedObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(draggedName));
                if(draggedObj) {
                    draggedObj->SetParentName(node.name);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void ScenePanel::RenderHierarchyNode(const GameObjectNode& node) {
    auto* obj = Scene::GetGameObjectByIndex(node.index);
    if(!obj) return;

    ImGui::PushID(node.index);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, StyleColors::HOVER);

    bool isSelected = selectedObjectName == node.name;
    if(isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Header, StyleColors::SELECTED);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, StyleColors::SELECTED_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, StyleColors::SELECTED_ACTIVE);
    }

    bool hasChildren = !node.children.empty();
    ImGuiTreeNodeFlags flags = isSelected ? ImGuiTreeNodeFlags_Selected : 0;
    if(!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

    if(isSelected) {
        ImGui::PopStyleColor(4);
    } else {
        ImGui::PopStyleColor(1);
    }

    if(ImGui::IsItemClicked()) {
        SelectObject(node.name);
    }

    RenderContextMenu(node);
    HandleDragDrop(node);

    if(nodeOpen && hasChildren) {
        for(const auto& child : node.children) {
            RenderHierarchyNode(child);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void ScenePanel::SelectObject(const std::string& name) {
    selectedObjectName = name;
}

void ScenePanel::EnsureValidSelection(const std::vector<GameObjectNode>& hierarchy) {
    if(!selectedObjectName.empty()) {
        int selectedIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
        if(selectedIndex == Scene::ABSENT_OBJECT) {
            selectedObjectName = "";
        }
    }

    if(selectedObjectName.empty() && !hierarchy.empty()) {
        selectedObjectName = hierarchy[0].name;
    }
}

void ScenePanel::Render() {
    RenderDefaultPanel();
    ImGui::SetCursorPosY(38.0f);

    if(!Scene::IsInitialized()) {
        ImGui::Text("Scene not initialized");
        ImGui::End();
        return;
    }

    bool canDelete = !selectedObjectName.empty() &&
                     Scene::GetGameObjectIndexByName(selectedObjectName) != Scene::ABSENT_OBJECT;
    if(ImGui::IsKeyPressed(ImGuiKey_Delete) && canDelete) {
        DeleteSelectedObject();
    }

    auto hierarchy = BuildHierarchy();
    EnsureValidSelection(hierarchy);

    if(hierarchy.empty()) {
        ImGui::Text("No objects in scene");

        if(ImGui::BeginPopupContextWindow("SceneEmptyContextMenu",
           ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if(ImGui::MenuItem("Create GameObject")) {
                GameObjectParameters params;
                params.name = "GameObject";
                uint newObjectIndex = Scene::CreateGameObject(params);
                auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
                if(newObj) {
                    SelectObject(newObj->GetName());
                }
            }
            ImGui::EndPopup();
        }

        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_NODE")) {
                std::string draggedName(static_cast<const char*>(payload->Data));
                auto* draggedObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(draggedName));
                if(draggedObj) {
                    draggedObj->SetParentName(UNDEFINED_NAME);
                }
            }
            ImGui::EndDragDropTarget();
        }
    } else {
        ImGui::BeginChild("SceneHierarchy", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for(const auto& rootNode : hierarchy) {
            RenderHierarchyNode(rootNode);
        }

        ImGui::Spacing();
        ImGui::InvisibleButton("EmptyDropTarget", ImVec2(-1, ImGui::GetTextLineHeight() * 2));

        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_NODE")) {
                std::string draggedName(static_cast<const char*>(payload->Data));
                auto* draggedObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(draggedName));
                if(draggedObj) {
                    draggedObj->SetParentName(UNDEFINED_NAME);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if(ImGui::BeginPopupContextWindow("SceneHierarchyContextMenu",
           ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if(ImGui::MenuItem("Create GameObject")) {
                GameObjectParameters params;
                params.name = "GameObject";
                uint newObjectIndex = Scene::CreateGameObject(params);
                auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
                if(newObj) {
                    SelectObject(newObj->GetName());
                }
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }

    ImGui::End();
}

std::string ScenePanel::GetSelectedObjectName() const {
    return selectedObjectName;
}

void ScenePanel::DeleteSelectedObject() {
    if(selectedObjectName.empty()) {
        return;
    }

    int objectIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
    if(objectIndex == Scene::ABSENT_OBJECT) {
        return;
    }

    uint deletedIndex = static_cast<uint>(objectIndex);
    uint lastIndex = Scene::GetLastGameObjectIndex();
    Scene::RemoveGameObject(deletedIndex);

    selectedObjectName = "";
    if(Scene::GetLastGameObjectIndex() != Scene::ABSENT_OBJECT_NUMBER) {
        if(deletedIndex < lastIndex) {
            auto* nextObj = Scene::GetGameObjectByIndex(static_cast<int>(deletedIndex));
            if(nextObj) {
                selectedObjectName = nextObj->GetName();
            }
        }

        if(selectedObjectName.empty() && deletedIndex > 0) {
            auto* prevObj = Scene::GetGameObjectByIndex(static_cast<int>(deletedIndex - 1));
            if(prevObj) {
                selectedObjectName = prevObj->GetName();
            }
        }
    }
}