#pragma once

#include "../EditorPanel.h"
#include "SelectionManager.h"
#include "SceneHierarchyRenderer.h"
#include <memory>

class ScenePanel : public EditorPanel {
private:
    SelectionManager selectionManager;
    std::unique_ptr<SceneHierarchyRenderer> renderer;

public:
    ScenePanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    NODISCARD std::string GetSelectedObjectName() const;
};