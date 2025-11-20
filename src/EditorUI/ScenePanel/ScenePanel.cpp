#include "ScenePanel.h"

#include "SceneHierarchyBuilder.h"
#include "../../Game/Scene/Scene.h"

ScenePanel::ScenePanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("Scene", pos, size),
      renderer(std::make_unique<SceneHierarchyRenderer>(selectionManager)) {
}

void ScenePanel::Render() {
    RenderDefaultPanel();
    ImGui::SetCursorPosY(38.0f);

    if(!Scene::IsInitialized()) {
        ImGui::Text("Scene not initialized");
        ImGui::End();
        return;
    }

    bool canDelete = selectionManager.HasSelection() &&
                     Scene::GetGameObjectIndexByName(selectionManager.GetSelected()) != Scene::ABSENT_OBJECT;
    if(ImGui::IsKeyPressed(ImGuiKey_Delete) && canDelete) {
        selectionManager.DeleteSelected();
    }

    auto hierarchy = SceneHierarchyBuilder::Build();
    selectionManager.EnsureValid(hierarchy);
    renderer->Render(hierarchy);

    ImGui::End();
}

std::string ScenePanel::GetSelectedObjectName() const {
    return selectionManager.GetSelected();
}