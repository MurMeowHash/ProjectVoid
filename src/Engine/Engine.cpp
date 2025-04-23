#include "Engine.h"
#include "../Debug/Debug.h"
#include "../Core/Core.h"
#include "Time/Time.h"
#include "Input/Input.h"
#include "../Game/Scene/Scene.h"
#include "../Renderer/Renderer.h"
#include "../Core/Resources/ResourceManager.h"
#include "Physics/Physics.h"

namespace Engine {
    ExecutionMode executionMode;
    float fpsUpdateFrequency = 3.0f;
    float currentTime = 0.0f;

    void DebugFps();

    void Initialize(ExecutionMode mode) {
        executionMode = mode;
        Debug::Initialize(&std::cout);
        Core::Initialize(1920, 1080, "Project Void", false);
        Input::Initialize();
        Input::SetCursorLock(true);

        ResourceManager::LoadAssets();
        Scene::LoadScene();
        Physics::Initialize();
        Renderer::Initialize();
    }

    void Run() {
        while(!Core::WindowShouldClose()) {
            Time::UpdateDeltaTime();
            Physics::Update();
            Scene::Update();
            Renderer::RenderFrame();

            if(Input::GetKeyDown(InputKey::KeyEscape)) {
                glfwSetWindowShouldClose(Core::GetActiveWindow(), true);
            }

            Input::Update();
            Core::FinishFrame();

            DebugFps();
        }

        Dispose();
    }

    void SetFpsUpdateFrequency(float time) {
        if(time <= 0.0f) return;

        fpsUpdateFrequency = time;
    }

    bool IsDebugMode() {
        return executionMode == ExecutionMode::Debug;
    }

    void Dispose() {
        Renderer::Dispose();
        Physics::Dispose();
        Scene::Dispose();
        ResourceManager::Dispose();
        Core::Dispose();
    }

    void DebugFps() {
        if(!IsDebugMode()) return;

        currentTime += Time::GetDeltaTime();
        if(currentTime >= fpsUpdateFrequency) {
            auto fps = 1.0f / Time::GetDeltaTime();
            Debug::Log("FPS: ", fps, '\n');
            currentTime = 0.0f;
        }
    }
}