#pragma once
#include "../Debug/Debug.h"
#include "../Engine/Engine.h"

namespace Error {
    template<typename ... Args>
    void FallWithFatalError(Args ... args) {
        Debug::LogError(args...);
        Engine::Dispose();
        exit(1);
    }
}