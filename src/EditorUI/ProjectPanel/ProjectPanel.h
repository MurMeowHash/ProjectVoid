#pragma once
#include "../EditorPanel.h"
#include "FileItem.h"
#include <imgui/imgui.h>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

class ProjectPanel : public EditorPanel {
private:
    std::string projectDirectory;
    std::string currentDirectory;
    
    void NavigateToDirectory(const std::filesystem::path& path);
    void NavigateBack();
    NODISCARD bool CanNavigateBack() const;
    NODISCARD bool IsParentPathValid(const std::filesystem::path& currentPath, const std::filesystem::path& projectPath) const;
    void OpenDirectoryInExplorer(const std::filesystem::path& path) const;
public:
    ProjectPanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    void SetProjectDirectory(const std::string& directory);
};

