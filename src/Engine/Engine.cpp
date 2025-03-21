#include "Engine.h"
#include "../Debug/Debug.h"
#include "../Core/Core.h"
#include "Time/Time.h"
#include "Input/Input.h"
#include "../Game/Scene/Scene.h"
#include "../Renderer/Renderer.h"
#include "../Core/Resources/ResourceManager.h"

namespace Engine {
    ExecutionMode executionMode;

    RenderData ConstructRenderData();

    //TODO: review
    RenderData ConstructRenderData() {
        RenderData renderData;
        renderData.geometryRenderItems = Scene::GetGeometryRenderItems();
        auto cameraObject = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(Scene::GetMainCameraName()));
        if(cameraObject) {
            auto camera = cameraObject->GetComponent<Camera>();
            if(camera) {
                renderData.mainCameraData.projectionMatrix = camera->GetProjectionMatrix();
                renderData.mainCameraData.viewMatrix = camera->GetViewMatrix();
            }
        }

        return renderData;
    }

    void Initialize(ExecutionMode mode) {
        executionMode = mode;
        Debug::Initialize(&std::cout);
        Core::Initialize(800, 600, "Project Void", true);
        Input::Initialize();
        Input::SetCursorLock(true);

        ResourceManager::LoadAssets();
        Scene::LoadScene();
        Renderer::Initialize();
    }

    void Run() {
        while(!Core::WindowShouldClose()) {
            Time::UpdateDeltaTime();
            Scene::Update();
            Renderer::RenderFrame(ConstructRenderData());

            if(Input::GetKeyDown(InputKey::KeyEscape)) {
                glfwSetWindowShouldClose(Core::GetActiveWindow(), true);
            }

            Input::Update();
            Core::FinishFrame();
        }

        Dispose();
    }

    bool IsDebugMode() {
        return executionMode == ExecutionMode::Debug;
    }

    void Dispose() {
        Renderer::Dispose();
        Scene::Dispose();
        ResourceManager::Dispose();
        Core::Dispose();
    }
}