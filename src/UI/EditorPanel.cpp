#include "EditorPanel.h"
#include "EditorStyles.h"

#include <utility>

EditorPanel::EditorPanel(std::string  panelName, const ImVec2& pos, const ImVec2& panelSize)
    : name(std::move(panelName)), position(pos), size(panelSize) {
}

void EditorPanel::DrawRectangle(const ImVec2& pos, float endX, float endY, const ImVec4& cul, const ImVec4& cbl, const ImVec4& cur, const ImVec4& cbr) {
    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
        pos,
        ImVec2(endX, endY),
        ImGui::GetColorU32(cul),
        ImGui::GetColorU32(cur),
        ImGui::GetColorU32(cbr),
        ImGui::GetColorU32(cbl)
    );
}

void EditorPanel::RenderDefaultPanel() {
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin(name.c_str(), nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    position = ImGui::GetWindowPos();
    size = ImGui::GetWindowSize();

    DrawRectangle(position, position.x + size.x, position.y + size.y, 
                 EditorStyles::Panel::BG_TOP_LEFT,
                 EditorStyles::Panel::BG_BOTTOM_LEFT,
                 EditorStyles::Panel::BG_TOP_RIGHT,
                 EditorStyles::Panel::BG_BOTTOM_RIGHT);

    DrawRectangle(position, position.x + size.x, position.y + 30.0f, 
                 EditorStyles::Panel::HEADER_TOP_LEFT,
                 EditorStyles::Panel::HEADER_BOTTOM_LEFT,
                 EditorStyles::Panel::HEADER_TOP_RIGHT,
                 EditorStyles::Panel::HEADER_BOTTOM_RIGHT);

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
