#pragma once

#include <string>
#include <vector>

struct GameObjectNode {
    std::string name;
    int index;
    std::vector<GameObjectNode> children;
};

class SelectionManager {
private:
    std::string selectedObjectName;

public:
    void Select(const std::string& name);
    void Clear();
    void DeleteSelected();
    void EnsureValid(const std::vector<GameObjectNode>& hierarchy);

    const std::string& GetSelected() const;
    bool IsSelected(const std::string& name) const;
    bool HasSelection() const;
};

