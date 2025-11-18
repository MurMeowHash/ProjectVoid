#pragma once
#include "../EditorPanel.h"
#include <imgui/imgui.h>
#include <string>
#include <vector>
#include <map>

class ScenePanel : public EditorPanel {
private:
    std::string selectedObjectName;
    
    struct GameObjectNode {
        std::string name;
        int index;
        std::vector<GameObjectNode> children;
    };
    
    std::vector<GameObjectNode> BuildHierarchy();
    void RenderHierarchyNode(const GameObjectNode& node);
    NODISCARD bool IsDescendant(const std::string& potentialParent, const std::string& child) const;
    NODISCARD bool CanSetParent(const std::string& childName, const std::string& potentialParentName) const;
    void DeleteSelectedObject();
    
public:
    ScenePanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    NODISCARD std::string GetSelectedObjectName() const;
};
