#include "TitleBarPanel.h"
#include "../EditorStyles.h"
#include "../../Core/Resources/ResourceManager.h"
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
    
    const float buttonSize = 12.0f;
    const float buttonSpacing = 8.0f;
    const float padding = 8.0f;
    
    // Меню зліва
    const float menuButtonSize = 30.0f;
    ImGui::SetCursorPos(ImVec2(padding, (size.y - menuButtonSize) * 0.5f));
    DrawMenuButton();
    ImGui::Dummy(ImVec2(menuButtonSize, menuButtonSize)); // Розширює межі вікна
    
    // Назва проекту по центру
    ImGui::SetCursorPosX(size.x * 0.5f - 100.0f);
    ImGui::SetCursorPosY((size.y - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::Text("ProjectVoid - Rat Racing Engine");
    
    ImGui::End();
}

void TitleBarPanel::DrawMenuButton() {
    const float buttonSize = 30.0f;
    ImVec2 buttonPos = ImGui::GetCursorPos();
    ImVec2 screenPos = ImVec2(position.x + buttonPos.x, position.y + buttonPos.y);

    ImGui::InvisibleButton("MenuButton", ImVec2(buttonSize, buttonSize));
    bool clicked = ImGui::IsItemClicked();
    
    int menuIconIndex = ResourceManager::GetTextureIndexByName("Menu");
    if (menuIconIndex) {
        Texture2D* menuTexture = ResourceManager::GetTextureByIndex(menuIconIndex);
        if (menuTexture) {
            ImTextureID menuIcon = menuTexture->GetHandle();
            const ImTextureRef texRef(menuIcon);
            ImVec2 iconPos = ImVec2(screenPos.x + buttonSize * 0.5f - buttonSize * 0.4f,
                                   screenPos.y + buttonSize * 0.5f - buttonSize * 0.4f);
            ImVec2 iconSize = ImVec2(buttonSize * 0.8f, buttonSize * 0.8f);
            ImGui::GetWindowDrawList()->AddImage(
                texRef,
                iconPos,
                ImVec2(iconPos.x + iconSize.x, iconPos.y + iconSize.y),
                ImVec2(0, 0),
                ImVec2(1, 1)
            );
        }
    }
    
    if (clicked) {
        ImGui::OpenPopup("MenuContext");
    }
    
    ShowContextMenu();
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

