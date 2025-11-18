#pragma once
#include <imgui/imgui.h>
#include <string>
#include "../Utils/Utils.h"

class EditorPanel {
protected:
    std::string name;
    ImVec2 position;
    ImVec2 size;

    void DrawRectangle(const ImVec2& pos, float x, float y, const ImVec4& leftColor, const ImVec4& rightColor);
    void RenderDefaultPanel();

public:
    EditorPanel(std::string  panelName, const ImVec2& pos, const ImVec2& panelSize);
    virtual ~EditorPanel() = default;

    virtual void Render() = 0;
    
    void SetPosition(const ImVec2& pos);
    void SetSize(const ImVec2& panelSize);
    NODISCARD ImVec2 GetPosition() const;
    NODISCARD ImVec2 GetSize() const;
    NODISCARD std::string GetName() const;
};
