#include "ConsolePanel.h"
#include <sstream>

std::vector<LogEntry> ConsolePanel::logs;

ConsolePanel::ConsolePanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("Console", pos, size) {
}

void ConsolePanel::AddLog(const std::string& message, LogType type) {
    LogEntry entry;
    entry.message = message;
    entry.type = type;
    logs.push_back(entry);
    if(logs.size() > MAX_LOGS) {
        logs.erase(logs.begin());
    }
}

void ConsolePanel::Render() {
    RenderDefaultPanel();

    ImGui::SetCursorPosY(35.0f);
    
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false);

    for(const auto& log : logs) {
        ImVec4 color;
        switch(log.type) {
            case LogType::Error:
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                break;
            case LogType::Warning:
                color = ImVec4(1.0f, 0.8f, 0.4f, 1.0f);
                break;
            case LogType::Info:
            default:
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                break;
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextWrapped("%s", log.message.c_str());
        ImGui::PopStyleColor();
    }

    if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::End();
}

