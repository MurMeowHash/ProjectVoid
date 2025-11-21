#pragma once
#include "../EditorPanel.h"
#include <imgui/imgui.h>
#include <string>
#include <glm/glm.hpp>

class GameObject;
class ObjectComponent;

class InspectorPanel : public EditorPanel {
private:

    std::string selectedObjectName;
    char nameBuffer[256];
    bool nameBufferDirty;

    void RenderObjectName(GameObject* obj);
    void RenderContextMenu(GameObject* obj);
    void RenderComponents(GameObject* obj);
    void RenderAddComponentMenu(GameObject* obj);

public:
    InspectorPanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    void SetSelectedObject(const std::string& objectName);
};
