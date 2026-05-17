#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include "../Engine/Engine.h"

namespace Debug {
    extern std::ostream *debugStream;

    void Initialize(std::ostream *targetStream);
    
    template<typename ... Args>
    void Log(const Args& ... args) {
        if(!Engine::IsDebugMode()) return;

        std::ostringstream oss;
        ((oss << args), ...);
        (*debugStream) << oss.str();
    }

    template<typename ... Args>
    void LogCritical(const std::string &criticalType, const Args& ... args) {
        if(!Engine::IsDebugMode()) return;

        std::ostringstream oss;
        oss << criticalType;
        ((oss << " > " << args), ...);
        oss << '\n';
        (*debugStream) << oss.str();
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
