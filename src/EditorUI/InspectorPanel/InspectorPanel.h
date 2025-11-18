#pragma once
#include "../EditorPanel.h"
#include <imgui/imgui.h>
#include <string>
#include <glm/glm.hpp>

class InspectorPanel : public EditorPanel {
private:
    std::string selectedObjectName;
    char nameBuffer[256];
    bool nameBufferDirty;
    glm::vec3 cachedPosition;
    glm::vec3 cachedRotation;
    glm::vec3 cachedScale;
    bool positionActive;
    bool rotationActive;
    bool scaleActive;
    
public:
    InspectorPanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    void SetSelectedObject(const std::string& objectName);
};

