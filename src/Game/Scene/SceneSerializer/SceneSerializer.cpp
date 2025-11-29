#include "SceneSerializer.h"
#include <fstream>
#include "../../../Debug/Debug.h"
#include "../Scene.h"
#include "../../Types/GameObject/GameObject.h"
#include <nlohmann/json.hpp>

namespace SceneSerializer {
    nlohmann::json SerializeGameObject(const GameObject* obj) {
        if(!obj) return nlohmann::json::object();
        return obj->SerializeToJson();
    }

    nlohmann::json SerializeScene() {
        nlohmann::json sceneJson;
        sceneJson["name"] = "DefaultScene";

        nlohmann::json objectsArray = nlohmann::json::array();

        for(uint i = 0; i <= Scene::GetLastGameObjectIndex(); ++i) {
            auto obj = Scene::GetGameObjectByIndex(static_cast<int>(i));
            if(obj) {
                objectsArray.push_back(SerializeGameObject(obj));
            }
        }
        
        sceneJson["objects"] = objectsArray;
        
        return sceneJson;
    }

    void WriteJsonFile(const std::string &filePath, const nlohmann::json &data) {
        std::ofstream file(filePath);
        if(!file.is_open()) {
            Debug::LogError("SceneSerializer", "Failed to open file for writing", filePath);
            return;
        }
        
        file << data.dump(2);
        file.close();
    }

    void SerializeSceneToFile(const std::string &filePath) {
        auto sceneJson = SerializeScene();
        WriteJsonFile(filePath, sceneJson);
    }
}

