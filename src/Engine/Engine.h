#pragma once
#include "ExecutionMode.h"
#include "../Utils/Utils.h"

namespace Engine {
    void Initialize(ExecutionMode mode);
    void Run();
    void SetFpsUpdateFrequency(float time);
    NODISCARD bool IsDebugMode();
    void Dispose();
}