#include "SceneHierarchyRenderer.h"

#include "SceneHierarchyValidator.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Game/ComponentScripts/ComponentRegistry.h"
#include "../../Utils/Utils.h"

SceneHierarchyRenderer::SceneHierarchyRenderer(SelectionManager& manager)
    : selectionManager(manager) {
}

void SceneHierarchyRenderer::RenderAddComponentMenu(int objectIndex) {
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

void SceneHierarchyRenderer::RenderSetParentMenu(const GameObjectNode& node) {
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

void SceneHierarchyRenderer::RenderContextMenu(const GameObjectNode& node) {
    if(!ImGui::BeginPopupContextItem()) return;

    if(ImGui::MenuItem("Create GameObject")) {
        GameObjectParameters params;
        params.name = "GameObject";
        params.parentName = node.name;
        uint newObjectIndex = Scene::CreateGameObject(params);
        auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
        if(newObj) {
            if(!params.parentName.empty() && params.parentName != UNDEFINED_NAME) {
                newObj->SetParentName(params.parentName);
            }
            selectionManager.Select(newObj->GetName());
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
        selectionManager.Select(node.name);
        selectionManager.DeleteSelected();
    }

    ImGui::EndPopup();
}

void SceneHierarchyRenderer::HandleDragDrop(const GameObjectNode& node) {
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

void SceneHierarchyRenderer::RenderNode(const GameObjectNode& node) {
    auto* obj = Scene::GetGameObjectByIndex(node.index);
    if(!obj) return;

    ImGui::PushID(node.index);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, StyleColors::HOVER);

    bool isSelected = selectionManager.IsSelected(node.name);
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
        selectionManager.Select(node.name);
    }

    RenderContextMenu(node);
    HandleDragDrop(node);

    if(nodeOpen && hasChildren) {
        for(const auto& child : node.children) {
            RenderNode(child);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void SceneHierarchyRenderer::Render(const std::vector<GameObjectNode>& hierarchy) {
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
                    selectionManager.Select(newObj->GetName());
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
            RenderNode(rootNode);
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
                    selectionManager.Select(newObj->GetName());
                }
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }
}

