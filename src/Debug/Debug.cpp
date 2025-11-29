#include "Debug.h"
#include "../UI/ConsolePanel/ConsolePanel.h"

namespace Debug {
    std::ostream *debugStream;

    void Initialize(std::ostream *targetStream) {
        debugStream = targetStream;
    }

    void AddLogToConsole(const std::string& message, int logType) {
        LogType type = LogType::Info;
        if(logType == 1) type = LogType::Warning;
        else if(logType == 2) type = LogType::Error;
        
        ConsolePanel::AddLog(message, type);
    }
}
