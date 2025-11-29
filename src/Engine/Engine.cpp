#include "Engine.h"
#include "../Debug/Debug.h"
#include "../Core/Core.h"
#include "Time/Time.h"
#include "Input/Input.h"
#include "../Game/Scene/Scene.h"
#include "../Game/Scene/SceneDeserializer/SceneDeserializer.h"
#include "../Game/Scene/SceneSerializer/SceneSerializer.h"
#include "../Renderer/Renderer.h"
#include "../Core/Resources/ResourceManager.h"
#include "Physics/Physics.h"
#include "../Game/ObjectGroup/ObjectGroupManager.h"
#include "../UI/EditorManager.h"
#include <imgui/imgui.h>

namespace Engine {
    ExecutionMode executionMode;
    float fpsUpdateFrequency = 3.0f;
    float currentTime = 0.0f;

    void DebugFps();

    void Initialize(ExecutionMode mode) {
        executionMode = mode;
        Debug::Initialize(&std::cout);
        Core::Initialize(1920, 1200, "Project Void", false);
        Input::Initialize();

        ResourceManager::LoadAssets();
        ObjectGroupManager::Initialize();
        Physics::Initialize();

        Scene::LoadScene();
        
        Scene::Start();
        Renderer::Initialize();

        Core::SetUsingMode(Core::UsingMode::UI);
        Input::SetCursorLock(false);

        EditorManager::Initialize();
    }

    void Run() {
        while(!Core::WindowShouldClose()) {
            Time::UpdateDeltaTime();
            Physics::Update();
            Scene::Update();
            Renderer::RenderFrame();

            EditorManager::Update();
            EditorManager::Render();

            Input::Update();
            Core::FinishFrame();

//            DebugFps();
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
        EditorManager::Dispose();
        Renderer::Dispose();
        Scene::Dispose();
        Physics::Dispose();
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