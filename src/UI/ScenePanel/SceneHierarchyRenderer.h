#pragma once

#include "SelectionManager.h"
#include <imgui/imgui.h>

class SceneHierarchyRenderer {
private:

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

