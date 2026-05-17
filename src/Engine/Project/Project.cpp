#include "Project.h"
#include "../Time/Time.h"
#include "../Input/Input.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Scene/SceneDeserializer/SceneDeserializer.h"
#include "../../Renderer/Renderer.h"
#include "../Physics/Physics.h"
#include "../../Core/Core.h"
#include "../../Debug/Debug.h"
#include "../ExecutionMode.h"
#include "../Engine.h"
#include "../../Core/Resources/ResourceManager.h"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace Project {
    static std::string projectDirectory;
    static std::vector<std::string> scenePaths;
    static std::string currentScene;

    void Initialize() {
        Physics::Initialize();
        Renderer::Initialize();
    }

    void Load(const std::string& directory) {
        projectDirectory = directory;
        scenePaths.clear();
        currentScene.clear();

        if (!fs::exists(projectDirectory) || !fs::is_directory(projectDirectory)) {
            return;
        }

        std::string scenesDirectory = projectDirectory + "/Scenes";
        if (fs::exists(scenesDirectory) && fs::is_directory(scenesDirectory)) {
            for (const auto& entry : fs::directory_iterator(scenesDirectory)) {
                if (entry.is_regular_file()) {
                    std::string extension = entry.path().extension().string();
                    if (extension == ".json" || extension == ".scene") {
                        scenePaths.push_back(entry.path().string());
                    }
                }
            }
        }

        if (!scenePaths.empty()) {
            currentScene = scenePaths[0];
            SceneDeserializer::DeserializeScene(SceneDeserializer::ReadJsonFile(currentScene));
        } else {
            Scene::LoadScene();
        }
        
        Scene::Start();
    }

    void Dispose() {
        projectDirectory.clear();
        scenePaths.clear();
        currentScene.clear();

        Renderer::Dispose();
        Scene::Dispose();
        Physics::Dispose();
    }

    void Run() {
        Time::UpdateDeltaTime();
        Physics::Update();
        Scene::Update();
        Renderer::RenderFrame();

        Input::Update();
        Core::FinishFrame();
    }

    const std::string& GetDirectory() {
        return projectDirectory;
    }

    const std::vector<std::string>& GetScenePaths() {
        return scenePaths;
    }

    const std::string& GetCurrentScene() {
        return currentScene;
    }

    void SetCurrentScene(const std::string& scenePath) {
        for (const auto& path : scenePaths) {
            if (path == scenePath) {
                currentScene = scenePath;
                return;
            }
        }
    }
}
