#include "Engine.h"
#include "../Debug/Debug.h"
#include "../Core/Core.h"
#include "Time/Time.h"

namespace Engine {
    ExecutionMode executionMode;

    void Initialize(ExecutionMode mode) {
        executionMode = mode;
        Debug::Initialize(&std::cout);
        Core::Initialize(1920, 1080, "Project Void", true);
    }

    void Run() {
        while(!Core::WindowShouldClose()) {
            Time::UpdateDeltaTime();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Debug::Log(Time::GetDeltaTime(), '\n');

            Core::FinishFrame();
        }

        CleanUp();
    }

    bool IsDebugMode() {
        return executionMode == ExecutionMode::Debug;
    }

    void CleanUp() {
        Debug::Log("Cleaning up...");
        Core::Terminate();
    }
}