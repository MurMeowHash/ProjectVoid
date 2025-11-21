#include "TitleBarPanel.h"
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
    
    // Фон панелі
    DrawRectangle(position, position.x + size.x, position.y + size.y, 
                 ImVec4(0.090f, 0.082f, 0.078f, 1.0f), 
                 ImVec4(0.129f, 0.129f, 0.129f, 1.0f));
    
    const float buttonSize = 12.0f;
    const float buttonSpacing = 8.0f;
    const float padding = 8.0f;
    
    // Меню зліва
    ImGui::SetCursorPos(ImVec2(padding, (size.y - buttonSize) * 0.5f));
    DrawMenuButton();
    ImGui::Dummy(ImVec2(buttonSize, buttonSize)); // Розширює межі вікна
    
    // Назва проекту по центру
    ImGui::SetCursorPosX(size.x * 0.5f - 100.0f);
    ImGui::SetCursorPosY((size.y - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::Text("ProjectVoid - Rat Racing Engine");
    
    // Кнопки управління вікном справа
    ImGui::SetCursorPosX(size.x - (buttonSize * 2 + buttonSpacing) - padding);
    ImGui::SetCursorPosY((size.y - buttonSize) * 0.5f);
    DrawWindowControls();
    ImGui::Dummy(ImVec2(buttonSize * 2 + buttonSpacing, buttonSize)); // Розширює межі вікна
    
    ImGui::End();
}

void TitleBarPanel::DrawMenuButton() {
    const float buttonSize = 12.0f;
    ImVec2 buttonPos = ImGui::GetCursorPos();
    ImVec2 screenPos = ImVec2(position.x + buttonPos.x, position.y + buttonPos.y);
    
    // Створюємо невидиму кнопку для hover detection
    ImGui::InvisibleButton("MenuButton", ImVec2(buttonSize, buttonSize));
    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();
    
    DrawSquareButton(screenPos, ImVec2(buttonSize, buttonSize), hovered, ImGui::IsPopupOpen("MenuContext"));
    
    if (clicked) {
        ImGui::OpenPopup("MenuContext");
    }
    
    ShowContextMenu();
}

void TitleBarPanel::DrawWindowControls() {
    const float buttonSize = 12.0f;
    const float buttonSpacing = 8.0f;
    ImVec2 startPos = ImGui::GetCursorPos();
    ImVec2 screenStartPos = ImVec2(position.x + startPos.x, position.y + startPos.y);
    
    // Кнопка мінімізації
    ImVec2 minimizePos = screenStartPos;
    ImGui::SetCursorPos(startPos);
    ImGui::InvisibleButton("MinimizeButton", ImVec2(buttonSize, buttonSize));
    bool minimizeHovered = ImGui::IsItemHovered();
    DrawSquareButton(minimizePos, ImVec2(buttonSize, buttonSize), minimizeHovered, false);
    
    // Кнопка максимізації
    ImVec2 maximizePos = ImVec2(screenStartPos.x + buttonSize + buttonSpacing, screenStartPos.y);
    ImGui::SetCursorPos(ImVec2(startPos.x + buttonSize + buttonSpacing, startPos.y));
    ImGui::InvisibleButton("MaximizeButton", ImVec2(buttonSize, buttonSize));
    bool maximizeHovered = ImGui::IsItemHovered();
    DrawSquareButton(maximizePos, ImVec2(buttonSize, buttonSize), maximizeHovered, false);
}

void TitleBarPanel::DrawSquareButton(const ImVec2& pos, const ImVec2& size, bool hovered, bool active) {
    ImVec4 color = hovered ? ImVec4(0.3f, 0.3f, 0.3f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    if (active) {
        color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    }
    
    ImGui::GetWindowDrawList()->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::GetColorU32(color)
    );
    
    // Рамка
    ImGui::GetWindowDrawList()->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)),
        0.0f, 0, 1.0f
    );
}

void TitleBarPanel::ShowContextMenu() {
    ImVec2 menuPos = ImVec2(position.x + 8.0f + 12.0f, position.y + size.y);
    ImGui::SetNextWindowPos(menuPos);
    
    if (ImGui::BeginPopup("MenuContext")) {
        if (ImGui::MenuItem("Save Scene")) {
            SceneSerializer::SerializeSceneToFile("../resources/Scenes/Scene.json");
            ImGui::CloseCurrentPopup();
        }
        
        if (ImGui::MenuItem("Exit")) {
            glfwSetWindowShouldClose(Core::GetActiveWindow(), true);
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

