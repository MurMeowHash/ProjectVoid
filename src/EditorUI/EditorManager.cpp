#include "EditorManager.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include "ScenePanel/ScenePanel.h"
#include "InspectorPanel/InspectorPanel.h"
#include "ProjectPanel/ProjectPanel.h"
#include "ConsolePanel/ConsolePanel.h"
#include "TitleBarPanel/TitleBarPanel.h"
#include "../Core/Core.h"
#include <memory>

namespace EditorManager {
    constexpr float PANEL_SIZE = 200.0f;
    constexpr float TITLE_BAR_HEIGHT = 30.0f;

    std::unique_ptr<TitleBarPanel> titleBarPanel;
    std::unique_ptr<ScenePanel> scenePanel;
    std::unique_ptr<InspectorPanel> inspectorPanel;
    std::unique_ptr<ProjectPanel> projectPanel;
    std::unique_ptr<ConsolePanel> consolePanel;

    void CreatePanels() {
        const ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        titleBarPanel = std::make_unique<TitleBarPanel>(
            ImVec2(0, 0),
            ImVec2(displaySize.x, TITLE_BAR_HEIGHT)
        );

        scenePanel = std::make_unique<ScenePanel>(
            ImVec2(displaySize.x - PANEL_SIZE * 2, TITLE_BAR_HEIGHT),
            ImVec2(PANEL_SIZE, displaySize.y - PANEL_SIZE - TITLE_BAR_HEIGHT)
        );

        inspectorPanel = std::make_unique<InspectorPanel>(
            ImVec2(displaySize.x - PANEL_SIZE, TITLE_BAR_HEIGHT),
            ImVec2(PANEL_SIZE, displaySize.y - PANEL_SIZE - TITLE_BAR_HEIGHT)
        );

        projectPanel = std::make_unique<ProjectPanel>(
            ImVec2(0, displaySize.y - PANEL_SIZE),
            ImVec2(displaySize.x - PANEL_SIZE * 2, PANEL_SIZE)
        );

        consolePanel = std::make_unique<ConsolePanel>(
            ImVec2(displaySize.x - PANEL_SIZE * 2, displaySize.y - PANEL_SIZE),
            ImVec2(PANEL_SIZE * 2, PANEL_SIZE)
        );
    }

    void Initialize() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplGlfw_InitForOpenGL(Core::GetActiveWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 420");

        ImGui::GetStyle().WindowBorderSize = 0.0f;
        CreatePanels();
    }

    void Update() {
        const ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        Core::SetViewport(0, static_cast<int>(PANEL_SIZE + TITLE_BAR_HEIGHT),
                         static_cast<int>(displaySize.x - PANEL_SIZE * 2),
                         static_cast<int>(displaySize.y - PANEL_SIZE - TITLE_BAR_HEIGHT));

        titleBarPanel->SetPosition(ImVec2(0, 0));
        titleBarPanel->SetSize(ImVec2(displaySize.x, TITLE_BAR_HEIGHT));

        scenePanel->SetPosition(ImVec2(displaySize.x - PANEL_SIZE * 2, TITLE_BAR_HEIGHT));
        scenePanel->SetSize(ImVec2(PANEL_SIZE, displaySize.y - PANEL_SIZE - TITLE_BAR_HEIGHT));

        inspectorPanel->SetPosition(ImVec2(displaySize.x - PANEL_SIZE, TITLE_BAR_HEIGHT));
        inspectorPanel->SetSize(ImVec2(PANEL_SIZE, displaySize.y - PANEL_SIZE - TITLE_BAR_HEIGHT));

        projectPanel->SetPosition(ImVec2(0, displaySize.y - PANEL_SIZE));
        projectPanel->SetSize(ImVec2(displaySize.x - PANEL_SIZE * 2, PANEL_SIZE));

        consolePanel->SetPosition(ImVec2(displaySize.x - PANEL_SIZE * 2, displaySize.y - PANEL_SIZE));
        consolePanel->SetSize(ImVec2(PANEL_SIZE * 2, PANEL_SIZE));
    }

    void Render() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        scenePanel->Render();
        inspectorPanel->SetSelectedObject(scenePanel->GetSelectedObjectName());
        inspectorPanel->Render();
        projectPanel->Render();
        consolePanel->Render();
        titleBarPanel->Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Dispose() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}