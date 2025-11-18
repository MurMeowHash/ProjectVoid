#include "FileItem.h"
#include "../../Utils/Utils.h"
#include <functional>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#ifdef CreateWindow
#undef CreateWindow
#endif

FileItem::FileItem(const std::filesystem::path& filePath, FileItemType itemType, ImTextureID texture, const ImVec2& size)
    : path(filePath), type(itemType), iconTexture(texture), buttonSize(size) {
    if(itemType == FileItemType::Directory) {
        isDirectory = true;
    } else if(itemType == FileItemType::File) {
        isDirectory = false;
    } else {
        isDirectory = is_directory(filePath);
    }
    fileName = path.filename().string();
    displayName = fileName;
}

FileItem::FileItem(const std::string& name, FileItemType itemType, ImTextureID texture, const ImVec2& size)
    : fileName(name), displayName(name), type(itemType), iconTexture(texture), buttonSize(size), isDirectory(itemType == FileItemType::Directory || itemType == FileItemType::ParentDirectory) {
    if(itemType != FileItemType::ParentDirectory) {
        path = std::filesystem::path(name);
    }
}

void FileItem::CalculateDisplayName(float maxTextWidth) {
    displayName = fileName;

    if(ImGui::CalcTextSize(displayName.c_str()).x <= maxTextWidth) {
        return;
    }
    
    const std::string ellipsis = "...";
    const float availableWidth = maxTextWidth - ImGui::CalcTextSize(ellipsis.c_str()).x;
    const int optimalLength = Utils::FindOptimalTextLength(displayName, availableWidth);
    TruncateTextWithEllipsis(optimalLength);
}

void FileItem::TruncateTextWithEllipsis(int optimalLength) {
    const std::string ellipsis = "...";
    
    if(optimalLength > 0) {
        displayName = displayName.substr(0, optimalLength) + ellipsis;
    } else {
        displayName = ellipsis;
    }
}

bool FileItem::Render() {
    std::string uniqueId = GenerateUniqueId();
    ImGui::PushID(uniqueId.c_str());

    ImGui::BeginGroup();

    const std::string buttonId = "##" + uniqueId;
    const bool clicked = RenderButton(buttonId);

    const ImVec2 buttonMin = ImGui::GetItemRectMin();
    const ImVec2 buttonMax = ImGui::GetItemRectMax();
    const ImVec2 textPosition = CalculateTextPosition(buttonMin, buttonMax, buttonMax.x - buttonMin.x);
    RenderText(textPosition);
    ShowTooltipIfHovered();
    
    ImGui::EndGroup();
    ImGui::PopID();

    HandleClick(clicked);

    return clicked;
}

std::string FileItem::GenerateUniqueId() const {
    if(type == FileItemType::ParentDirectory) {
        return "..parent";
    }
    return path.empty() ? fileName : path.string();
}

bool FileItem::RenderButton(const std::string& buttonId) const {
    if(iconTexture != 0) {
        RenderButtonWithTexture(buttonId);
    }
    else{
        RenderButtonWithoutTexture(buttonId);
    }
    return ImGui::IsItemClicked();

}

void FileItem::RenderButtonWithTexture(const std::string& buttonId) const {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    const ImTextureRef texRef(iconTexture);
    ImGui::ImageButton(buttonId.c_str(), texRef, buttonSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0));
    ImGui::PopStyleColor(3);
}

void FileItem::RenderButtonWithoutTexture(const std::string& buttonId) const {
    ImGui::PushStyleColor(ImGuiCol_Button, {0.5f, 0.5f, 0.5f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.6f, 0.6f, 0.6f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.4f, 0.4f, 0.4f, 1.0f});
    ImGui::Button(buttonId.c_str(), buttonSize);
    ImGui::PopStyleColor(3);
}

ImVec2 FileItem::CalculateTextPosition(const ImVec2& buttonMin, const ImVec2& buttonMax, float buttonWidth) {
    float maxTextWidth = buttonWidth - 4.0f;
    CalculateDisplayName(maxTextWidth);

    ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
    float buttonCenterX = buttonMin.x + buttonWidth * 0.5f;
    float textX = buttonCenterX - textSize.x * 0.5f;
    float textY = buttonMax.y + 5.0f;

    return {textX, textY};
}

void FileItem::RenderText(const ImVec2& position) const {
    ImGui::SetCursorScreenPos(position);
    ImGui::Text("%s", displayName.c_str());
}

void FileItem::ShowTooltipIfHovered() const {
    if(ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", fileName.c_str());
    }
}

void FileItem::HandleClick(bool clicked) const {
    if(clicked && type == FileItemType::File && !isDirectory) {
        Utils::OpenInExplorer(path.string());
    }
}

void FileItem::SetIconTexture(ImTextureID texture) {
    iconTexture = texture;
}

bool FileItem::IsDirectory() const {
    return isDirectory;
}

const std::filesystem::path& FileItem::GetPath() const {
    return path;
}

const std::string& FileItem::GetFileName() const {
    return fileName;
}

FileItemType FileItem::GetType() const {
    return type;
}
