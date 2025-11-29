#pragma once
#include "../EditorPanel.h"
#include <imgui/imgui.h>
#include <string>
#include <vector>

enum class LogType {
    Info,
    Warning,
    Error
};

struct LogEntry {
    std::string message;
    LogType type;
};

class ConsolePanel : public EditorPanel {
private:
    static std::vector<LogEntry> logs;
    static constexpr int MAX_LOGS = 1000;

public:
    ConsolePanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    static void AddLog(const std::string& message, LogType type = LogType::Info);
};
