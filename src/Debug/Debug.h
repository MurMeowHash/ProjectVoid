#pragma once
#include <iostream>
#include "../Engine/Engine.h"

namespace Debug {
    extern std::ostream *debugStream;

    void Initialize(std::ostream *targetStream);
    template<typename ... Args>
    void Log(Args ... args) {
        if(!Engine::IsDebugMode()) return;

        ((*debugStream << args), ...);
    }

    template<typename ... Args>
    void LogCritical(const std::string &criticalType, Args ... args) {
        if(!Engine::IsDebugMode()) return;

        *debugStream << criticalType;
        ((*debugStream << " > " << args), ...);
        *debugStream << '\n';
    }

    template<typename ... Args>
    void LogError(Args ... args) {
        LogCritical("Error", args...);
    }

    template<typename ... Args>
    void LogWarning(Args ... args) {
        LogCritical("Warning", args...);
    }
}