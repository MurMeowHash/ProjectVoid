#pragma once
#include <iostream>
#include "../Engine/Engine.h"

namespace Debug {
    extern std::ostream *debugStream;

    void Initialize(std::ostream *targetStream);
    template<typename ... Args>
    void Log(const Args& ... args) {
        if(!Engine::IsDebugMode()) return;

        ((*debugStream << args), ...);
    }

    template<typename ... Args>
    void LogCritical(const std::string &criticalType, const Args& ... args) {
        if(!Engine::IsDebugMode()) return;

        *debugStream << criticalType;
        ((*debugStream << " > " << args), ...);
        *debugStream << '\n';
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