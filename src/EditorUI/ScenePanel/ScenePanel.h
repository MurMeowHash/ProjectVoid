#pragma once
#include "../EditorPanel.h"
#include <string>
#include <vector>

class ScenePanel : public EditorPanel {
private:
    struct GameObjectNode {
        std::string name;
        int index;
        std::vector<GameObjectNode> children;
    };

    struct StyleColors {
        static constexpr ImVec4 HOVER = ImVec4(0.5f, 0.5f, 0.5f, 0.70f);
        static constexpr ImVec4 SELECTED = ImVec4(0.5f, 0.5f, 0.5f, 0.80f);
        static constexpr ImVec4 SELECTED_HOVER = ImVec4(0.6f, 0.6f, 0.6f, 0.90f);
        static constexpr ImVec4 SELECTED_ACTIVE = ImVec4(0.7f, 0.7f, 0.7f, 1.00f);
    };

    std::string selectedObjectName;

    std::vector<GameObjectNode> BuildHierarchy();
    void RenderHierarchyNode(const GameObjectNode& node);
    void RenderContextMenu(const GameObjectNode& node);
    void RenderAddComponentMenu(int objectIndex);
    void RenderSetParentMenu(const GameObjectNode& node);
    void HandleDragDrop(const GameObjectNode& node);
    void DeleteSelectedObject();
    void SelectObject(const std::string& name);
    void EnsureValidSelection(const std::vector<GameObjectNode>& hierarchy);

public:
    ScenePanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    NODISCARD std::string GetSelectedObjectName() const;
};