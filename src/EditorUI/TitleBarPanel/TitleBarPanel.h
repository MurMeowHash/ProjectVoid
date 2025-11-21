#pragma once

#include "../EditorPanel.h"
#include <imgui/imgui.h>
#include <string>

class TitleBarPanel : public EditorPanel {
public:
    explicit TitleBarPanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    
private:
    void DrawMenuButton();
    void DrawWindowControls();
    void DrawSquareButton(const ImVec2& pos, const ImVec2& size, bool hovered, bool active);
    void ShowContextMenu();
};

