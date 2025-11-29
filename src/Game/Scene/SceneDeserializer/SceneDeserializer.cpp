#include "SceneDeserializer.h"
#include <fstream>
#include <map>
#include "../../../Debug/Debug.h"
#include "../Scene.h"
#include "../../Types/CreateParameters.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../ComponentScripts/Transform/Transform.h"
#include "../../ComponentScripts/MeshRenderData/MeshRenderData.h"
#include "../../ObjectGroup/ObjectGroupManager.h"
#include "../../../Core/Resources/ResourceManager.h"
#include "../../ComponentScripts/ComponentRegistry.h"
#include "../../../Utils/JsonUtils.h"

namespace SceneDeserializer {
    std::string ParseGameObjectName(const nlohmann::json& objJson) {
        if(!objJson.contains("name") || !objJson["name"].is_string()) {
            Debug::LogError("SceneDeserializer", "GameObject missing name");
            return "";
        }
        return objJson["name"].get<std::string>();
    }

    GameObjectParameters ParseGameObjectParameters(const nlohmann::json& objJson, const std::string& name) {
        GameObjectParameters params;
        params.name = name;
        // Transform тепер створюється автоматично в конструкторі GameObject
        // і може бути оновлений через масив components
        params.transform = Transform(); // Дефолтний Transform
        params.parentName = UNDEFINED_NAME;

        if(objJson.contains("group") && objJson["group"].is_string()) {
            std::string group = objJson["group"].get<std::string>();
            ObjectGroupManager::RegisterGroup(group);
            params.groupName = group;
        }

        return params;
    }

    bool ApplyModelToGameObject(GameObject* obj, const nlohmann::json& objJson) {
        if(!objJson.contains("model") || !objJson["model"].is_string()) {
            return true;
        }

        std::string modelName = objJson["model"].get<std::string>();
        int modelIndex = ResourceManager::GetModelIndexByName(modelName);
        if(modelIndex == ABSENT_RESOURCE) {
            Debug::LogError("SceneDeserializer", "Model not found", modelName);
            return false;
        }

        auto model = ResourceManager::GetModelByIndex(modelIndex);
        Scene::AddModelMeshesToGameObject(obj, model);
        return true;
    }

    void ApplyGroupToGameObject(GameObject* obj, const nlohmann::json& objJson) {
        if(objJson.contains("group") && objJson["group"].is_string()) {
            obj->SetGroup(objJson["group"].get<std::string>());
        }
    }

    void CreateComponentsFromJson(GameObject* obj, const nlohmann::json& componentsJson) {
        if(!componentsJson.is_array()) return;

        for(const auto& compJson : componentsJson) {
            if(!compJson.contains("type")) continue;

            std::string typeName = compJson["type"].get<std::string>();
            nlohmann::json params = compJson.contains("params") ? compJson["params"] : nlohmann::json::object();

            ComponentRegistry::Instance().Create(typeName, obj, params);
        }
    }

    int CreateGameObjectFromJson(const nlohmann::json &objJson, std::map<std::string, uint>& objectIndexMap) {
        const std::string name = ParseGameObjectName(objJson);
        if(name.empty()) {
            return Scene::ABSENT_OBJECT;
        }

        const GameObjectParameters params = ParseGameObjectParameters(objJson, name);
        const int objectIndex = static_cast<int>(Scene::CreateGameObject(params));

        const auto obj = Scene::GetGameObjectByIndex(objectIndex);
        if(!obj) {
            Debug::LogError("SceneDeserializer", "Failed to create GameObject", name);
            return Scene::ABSENT_OBJECT;
        }

        ApplyGroupToGameObject(obj, objJson);

        // Завантажуємо компоненти з масиву (включаючи Transform, якщо він там є)
        // Transform створюється автоматично в конструкторі GameObject,
        // але якщо він вказаний в масиві components, то Transform::CreateFromJson оновить його
        if(objJson.contains("components")) {
            CreateComponentsFromJson(obj, objJson["components"]);
        }

        if(!ApplyModelToGameObject(obj, objJson)) {
            return Scene::ABSENT_OBJECT;
        }

        // Якщо MeshRenderData існує, але modelName не встановлено, намагаємося знайти його
        auto* meshRenderData = obj->GetComponent<MeshRenderData>();
        if(meshRenderData && meshRenderData->modelName.empty() && !meshRenderData->meshes.empty()) {
            // Спочатку перевіряємо, чи є поле "model" в JSON
            if(objJson.contains("model") && objJson["model"].is_string()) {
                std::string modelName = objJson["model"].get<std::string>();
                // Перевіряємо, чи модель існує
                if(ResourceManager::GetModelIndexByName(modelName) != ABSENT_RESOURCE) {
                    meshRenderData->modelName = modelName;
                }
            } else {
                // Якщо поля "model" немає, намагаємося знайти за мешами
                std::string foundModelName = ResourceManager::GetModelNameByMeshes(meshRenderData->meshes);
                if(!foundModelName.empty()) {
                    meshRenderData->modelName = foundModelName;
                }
            }
        }

        objectIndexMap[name] = static_cast<uint>(objectIndex);

        return objectIndex;
    }

    nlohmann::json ReadJsonFile(const std::string &filePath) {
        std::ifstream file(filePath);
        if(!file.is_open()) {
            Debug::LogError("SceneDeserializer", "Failed to open file", filePath);
            return {};
        }
        
        nlohmann::json data;
        try {
            file >> data;
        } catch(const nlohmann::json::parse_error& e) {
            Debug::LogError("SceneDeserializer", "JSON parse error in", filePath, ":", e.what());
            return {};
        }
        
        return data;
    }

    void DeserializeScene(const nlohmann::json &scene) {
        if(scene.is_null() || scene.empty()) {
            Debug::LogError("SceneDeserializer", "Scene JSON is null or empty");
            return;
        }

        std::map<std::string, uint> objectIndexMap;
        std::vector<std::pair<int, std::string>> parentLinks;
        if(scene.contains("objects") && scene["objects"].is_array()) {
            for(auto &objJson : scene["objects"]) {
                int objectIndex = CreateGameObjectFromJson(objJson, objectIndexMap);

                if(objectIndex != Scene::ABSENT_OBJECT && objJson.contains("parent") && objJson["parent"].is_string()) {
                    std::string parentName = objJson["parent"];
                    parentLinks.emplace_back(objectIndex, parentName);
                }
            }
        }

        for(auto &link : parentLinks) {
            if(objectIndexMap.contains(link.second)) {
                auto obj = Scene::GetGameObjectByIndex(link.first);
                if(obj) {
                    obj->SetParentName(link.second);
                }
            } else {
                Debug::LogWarning("SceneDeserializer", "Parent not found", link.second);
            }
        }
        
        // Після встановлення всіх батьків, прив'язуємо Transform до батьків для всіх об'єктів
        // Це потрібно для випадків, коли об'єкт створювався з батьком, але батько ще не існував
        for(int i = 0; i <= static_cast<int>(Scene::GetLastGameObjectIndex()); ++i) {
            auto obj = Scene::GetGameObjectByIndex(i);
            if(obj && !obj->GetParentName().empty() && obj->GetParentName() != UNDEFINED_NAME) {
                auto* transform = obj->GetComponent<Transform>();
                if(transform) {
                    transform->AdjustToParent();
                }
            }
        }
    }
}
