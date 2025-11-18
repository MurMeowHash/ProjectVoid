#pragma once
#include <imgui/imgui.h>
#include <filesystem>
#include <string>
#include "../../Utils/Utils.h"

enum class FileItemType {
    Directory,
    File,
    ParentDirectory
};

class FileItem {
private:
    std::filesystem::path path;
    std::string fileName;
    std::string displayName;
    FileItemType type;
    bool isDirectory;
    ImTextureID iconTexture;
    ImVec2 buttonSize;
    
    void CalculateDisplayName(float maxTextWidth);
    void RenderButtonWithTexture(const std::string& buttonId) const;
    void RenderButtonWithoutTexture(const std::string& buttonId) const;
    ImVec2 CalculateTextPosition(const ImVec2& buttonMin, const ImVec2& buttonMax, float buttonWidth);
    void RenderText(const ImVec2& position) const;
    void ShowTooltipIfHovered() const;
    void HandleClick(bool clicked) const;
    void TruncateTextWithEllipsis(int optimalLength);
    NODISCARD std::string GenerateUniqueId() const;
    NODISCARD bool RenderButton(const std::string& buttonId) const;
public:
    explicit FileItem(const std::filesystem::path& filePath, FileItemType itemType = FileItemType::File, ImTextureID texture = 0, const ImVec2& size = ImVec2(70.0f, 58.0f));
    FileItem(const std::string& name, FileItemType itemType, ImTextureID texture = 0, const ImVec2& size = ImVec2(70.0f, 58.0f));
    
    NODISCARD bool Render();
    
    void SetIconTexture(ImTextureID texture);
    NODISCARD bool IsDirectory() const;
    NODISCARD const std::filesystem::path& GetPath() const;
    NODISCARD const std::string& GetFileName() const;
    NODISCARD FileItemType GetType() const;
};

