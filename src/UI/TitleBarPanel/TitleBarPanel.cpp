#include "TitleBarPanel.h"
#include "../EditorStyles.h"
#include "../../Game/Scene/SceneSerializer/SceneSerializer.h"
#include "../../Core/Core.h"
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>

TitleBarPanel::TitleBarPanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("TitleBar", pos, size) {
}

void TitleBarPanel::Render() {
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("TitleBar", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    position = ImGui::GetWindowPos();
    size = ImGui::GetWindowSize();

    DrawRectangle(position, position.x + size.x, position.y + size.y, 
                 EditorStyles::Panel::TITLEBAR_TOP_LEFT,
                 EditorStyles::Panel::TITLEBAR_BOTTOM_LEFT,
                 EditorStyles::Panel::TITLEBAR_TOP_RIGHT,
                 EditorStyles::Panel::TITLEBAR_BOTTOM_RIGHT);
    
    const float buttonWidth = 80.0f;
    const float buttonHeight = size.y; // Висота кнопок = висота TitleBar
    
    // Кнопки зліва без відступів
    float buttonsX = 0.0f;
    
    // Кнопка Save
    ImGui::SetCursorPos(ImVec2(buttonsX, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, EditorStyles::Button::TITLEBAR_DEFAULT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyles::Button::TITLEBAR_HOVERED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorStyles::Button::TITLEBAR_ACTIVE);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    if(ImGui::Button("Save", ImVec2(buttonWidth, buttonHeight))) {
        SceneSerializer::SerializeSceneToFile("../resources/Scenes/Scene.json");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    // Кнопка Exit
    buttonsX += buttonWidth;
    ImGui::SetCursorPos(ImVec2(buttonsX, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, EditorStyles::Button::TITLEBAR_DEFAULT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyles::Button::TITLEBAR_HOVERED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorStyles::Button::TITLEBAR_ACTIVE);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    if(ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
        glfwSetWindowShouldClose(Core::GetActiveWindow(), true);
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    ImGui::End();
}

