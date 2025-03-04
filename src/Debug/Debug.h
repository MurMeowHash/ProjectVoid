#pragma once
#include <iostream>
#include "../Engine/Engine.h"

namespace Debug {
    extern std::ostream *debugStream;

    void Initialize(std::ostream *targetStream);
    template<typename ... Args>
    void Log(Args ... args) {
        if(!Engine::IsDebugMode()) return;

        ((*debugStream << args << " "), ...);
    }

    template<typename ... Args>
    void LogError(Args ... args) {
        if(!Engine::IsDebugMode()) return;

        *debugStream << "Error";
        ((*debugStream << " > " << args), ...);
        *debugStream << '\n';
    }
}