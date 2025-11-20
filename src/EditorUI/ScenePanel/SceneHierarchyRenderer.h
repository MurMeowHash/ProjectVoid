#pragma once

#include "SelectionManager.h"
#include <imgui/imgui.h>

class SceneHierarchyRenderer {
private:
    struct StyleColors {
        static constexpr ImVec4 HOVER = ImVec4(0.5f, 0.5f, 0.5f, 0.70f);
        static constexpr ImVec4 SELECTED = ImVec4(0.5f, 0.5f, 0.5f, 0.80f);
        static constexpr ImVec4 SELECTED_HOVER = ImVec4(0.6f, 0.6f, 0.6f, 0.90f);
        static constexpr ImVec4 SELECTED_ACTIVE = ImVec4(0.7f, 0.7f, 0.7f, 1.00f);
    };

    SelectionManager& selectionManager;

    void RenderNode(const GameObjectNode& node);
    void RenderContextMenu(const GameObjectNode& node);
    void RenderAddComponentMenu(int objectIndex);
    void RenderSetParentMenu(const GameObjectNode& node);
    void HandleDragDrop(const GameObjectNode& node);

public:
    explicit SceneHierarchyRenderer(SelectionManager& manager);
    void Render(const std::vector<GameObjectNode>& hierarchy);
};

