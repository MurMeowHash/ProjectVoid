#include "EditorPanel.h"

#include <utility>

EditorPanel::EditorPanel(std::string  panelName, const ImVec2& pos, const ImVec2& panelSize)
    : name(std::move(panelName)), position(pos), size(panelSize) {
}

void EditorPanel::DrawRectangle(const ImVec2& pos, float endX, float endY, const ImVec4& leftColor, const ImVec4& rightColor) {
    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
        pos,
        ImVec2(endX, endY),
        ImGui::GetColorU32(leftColor),
        ImGui::GetColorU32(rightColor),
        ImGui::GetColorU32(rightColor),
        ImGui::GetColorU32(leftColor)
    );
}

void EditorPanel::RenderDefaultPanel() {
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    position = ImGui::GetWindowPos();
    size = ImGui::GetWindowSize();

    DrawRectangle(position, position.x + size.x, position.y + size.y, 
                 ImVec4(0.158f, 0.158f, 0.158f, 1.0f), 
                 ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f));
    
    DrawRectangle(position, position.x + size.x, position.y + 30.0f, 
                 ImVec4(0.090f, 0.082f, 0.078f, 1.0f), 
                 ImVec4(0.129f, 0.129f, 0.129f, 1.0f));

    ImGui::Text(name.c_str());
}

void EditorPanel::SetPosition(const ImVec2& pos) {
    position = pos;
}

void EditorPanel::SetSize(const ImVec2& panelSize) {
    size = panelSize;
}

ImVec2 EditorPanel::GetPosition() const {
    return position;
}

ImVec2 EditorPanel::GetSize() const {
    return size;
}

std::string EditorPanel::GetName() const {
    return name;
}
