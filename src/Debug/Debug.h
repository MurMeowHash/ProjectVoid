#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include "../Engine/Engine.h"

namespace Debug {
    extern std::ostream *debugStream;

    void Initialize(std::ostream *targetStream);
    void AddLogToConsole(const std::string& message, int logType);
    
    template<typename ... Args>
    void Log(const Args& ... args) {
        if(!Engine::IsDebugMode()) return;

        std::ostringstream oss;
        ((oss << args), ...);
        std::string logMessage = oss.str();
        
        (*debugStream) << logMessage;
        AddLogToConsole(logMessage, 0); // 0 = Info
    }

    template<typename ... Args>
    void LogCritical(const std::string &criticalType, const Args& ... args) {
        if(!Engine::IsDebugMode()) return;

        std::ostringstream oss;
        oss << criticalType;
        ((oss << " > " << args), ...);
        oss << '\n';
        std::string logMessage = oss.str();
        
        (*debugStream) << logMessage;
        
        int logType = 0; // Info
        if(criticalType == "Error") logType = 2; // Error
        else if(criticalType == "Warning") logType = 1; // Warning
        AddLogToConsole(logMessage, logType);
    }

    template<typename ... Args>
    void LogError(const Args& ... args) {
        LogCritical("Error", args...);
    }

    template<typename ... Args>
    void LogWarning(const Args& ... args) {
        LogCritical("Warning", args...);
    }
}