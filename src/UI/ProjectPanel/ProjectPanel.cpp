#include "ProjectPanel.h"
#include "../EditorStyles.h"
#include <algorithm>
#include <vector>
#include <string>
#include "../../Core/Resources/ResourceManager.h"
#include "../../Utils/Utils.h"

ProjectPanel::ProjectPanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("Project", pos, size), projectDirectory("../resources/"), currentDirectory("../resources/") {
}

void ProjectPanel::SetProjectDirectory(const std::string& directory) {
    if(std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
        projectDirectory = directory;
        currentDirectory = directory;
    }
}

void ProjectPanel::NavigateToDirectory(const std::filesystem::path& path) {
    if(exists(path) && is_directory(path)) {
        currentDirectory = path.string();
    }
}

bool ProjectPanel::IsParentPathValid(const std::filesystem::path& currentPath, const std::filesystem::path& projectPath) const {
    const std::filesystem::path parentPath = currentPath.parent_path();
    
    if(parentPath.empty() || parentPath == currentPath) {
        return false;
    }
    
    std::string parentStr;
    std::string projectStr;
    
    try {
        const std::filesystem::path canonicalParent = canonical(parentPath);
        const std::filesystem::path canonicalProject = canonical(projectPath);
        parentStr = Utils::NormalizePath(canonicalParent.string());
        projectStr = Utils::NormalizePath(canonicalProject.string());
    } catch(const std::filesystem::filesystem_error&) {
        parentStr = Utils::NormalizePath(parentPath.string());
        projectStr = Utils::NormalizePath(projectPath.string());
    }
    
    return parentStr == projectStr || parentStr.find(projectStr) == 0;
}

void ProjectPanel::NavigateBack() {
    if(currentDirectory.empty() || projectDirectory.empty()) {
        return;
    }
    const std::filesystem::path currentPath(currentDirectory);
    const std::filesystem::path projectPath(projectDirectory);
    if(IsParentPathValid(currentPath, projectPath)) {
        const std::filesystem::path parentPath = currentPath.parent_path();
        currentDirectory = parentPath.string();
    }
}

bool ProjectPanel::CanNavigateBack() const {
    if(currentDirectory.empty() || projectDirectory.empty()) {
        return false;
    }
    const std::filesystem::path currentPath(currentDirectory);
    const std::filesystem::path projectPath(projectDirectory);
    return currentPath == projectPath ? false : IsParentPathValid(currentPath, projectPath);
}

void ProjectPanel::Render() {
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

    constexpr float HEADER_HEIGHT = 30.0f;
    DrawRectangle(position, position.x + size.x, position.y + HEADER_HEIGHT, 
                 EditorStyles::Panel::HEADER_TOP_LEFT,
                 EditorStyles::Panel::HEADER_BOTTOM_LEFT,
                 EditorStyles::Panel::HEADER_TOP_RIGHT,
                 EditorStyles::Panel::HEADER_BOTTOM_RIGHT);

    // Назва панелі
    ImGui::SetCursorPosY((HEADER_HEIGHT - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::Text(name.c_str());

    // Кнопка "Open Folder" в правій частині заголовку
    const float buttonWidth = 120.0f;
    const float buttonHeight = HEADER_HEIGHT;
    float buttonX = size.x - buttonWidth;
    
    ImGui::SetCursorPos(ImVec2(buttonX, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, EditorStyles::Button::TITLEBAR_DEFAULT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyles::Button::TITLEBAR_HOVERED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorStyles::Button::TITLEBAR_ACTIVE);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    
    std::filesystem::path currentPath(currentDirectory);
    if(ImGui::Button("Open Folder", ImVec2(buttonWidth, buttonHeight))) {
        Utils::OpenInExplorer(currentPath.string());
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    ImGui::SetCursorPosY(38.0f);
    std::filesystem::path projectPath(projectDirectory);

    std::string projectName = projectPath.filename().string();
    projectName = projectName.empty() ? projectPath.parent_path().filename().string() : projectName;

    std::string relativePath = relative(currentPath, projectPath).string();
    relativePath = relativePath == "." ? "" : relativePath;

    ImGui::Text("%s %s", projectName.c_str(), relativePath.c_str());

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::BeginChild("ProjectFiles", ImVec2(0, 0), false);
    
    if(!currentDirectory.empty() && std::filesystem::exists(currentDirectory)) {
        try {
            std::vector<std::filesystem::directory_entry> entries;
            for(const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
                entries.push_back(entry);
            }

            std::ranges::sort(entries, [](const auto& a, const auto& b) {
                if(a.is_directory() != b.is_directory()) {
                    return a.is_directory() > b.is_directory();
                }
                return a.path().filename().string() < b.path().filename().string();
            });

            std::vector<std::unique_ptr<FileItem>> fileItems;

            ImTextureID folderIcon = ResourceManager::GetTextureByIndex(ResourceManager::GetTextureIndexByName("Folder"))->GetHandle();
            ImTextureID fileIcon = ResourceManager::GetTextureByIndex(ResourceManager::GetTextureIndexByName("File"))->GetHandle();
            ImTextureID pictureIcon = ResourceManager::GetTextureByIndex(ResourceManager::GetTextureIndexByName("Picture"))->GetHandle();

            if(CanNavigateBack()) {
                fileItems.push_back(std::make_unique<FileItem>(std::string(".."), FileItemType::ParentDirectory, folderIcon, ImVec2(70.0f, 70.0f)));
            }

            for(const auto& entry : entries) {
                FileItemType itemType = entry.is_directory() ? FileItemType::Directory : FileItemType::File;
                ImTextureID iconToUse;
                
                if(itemType == FileItemType::Directory) {
                    iconToUse = folderIcon;
                } else if(Utils::IsImageFile(entry.path())) {
                    iconToUse = pictureIcon;
                } else {
                    iconToUse = fileIcon;
                }
                
                fileItems.push_back(std::make_unique<FileItem>(entry.path(), itemType, iconToUse, ImVec2(70.0f, 70.0f)));
            }
            
            int columnsCount = static_cast<int>(ImGui::GetContentRegionAvail().x / 80);
            columnsCount = columnsCount < 1 ? 1 : columnsCount;

            for(int i = 0; i < fileItems.size(); i++) {
                if(fileItems[i]->Render()) {
                    if(fileItems[i]->GetType() == FileItemType::ParentDirectory) {
                        NavigateBack();
                    } else if(fileItems[i]->IsDirectory()) {
                        NavigateToDirectory(fileItems[i]->GetPath());
                    }
                }
                if(i + 1 < fileItems.size() && (i + 1) % columnsCount != 0) {
                    ImGui::SameLine(0.0f, 10);
                }
            }
        } catch(const std::filesystem::filesystem_error& e) {
            ImGui::Text("Error accessing directory");
        }
    } else if(!projectDirectory.empty() && std::filesystem::exists(projectDirectory)) {
        currentDirectory = projectDirectory;
    }
    
    ImGui::EndChild();
    ImGui::End();
}

