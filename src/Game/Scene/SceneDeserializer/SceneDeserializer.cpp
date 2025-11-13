#include "SceneDeserializer.h"
#include <fstream>
#include <map>
#include "../../../Debug/Debug.h"
#include "../Scene.h"
#include "../../Types/CreateParameters.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../ComponentScripts/Transform/Transform.h"
#include "../../ObjectGroup/ObjectGroupManager.h"
#include "../../../Core/Resources/ResourceManager.h"
#include <glm/glm.hpp>
#include "../../ComponentScripts/ComponentRegistry.h"
#include "../../../Utils/JsonUtils.h"

namespace SceneDeserializer {
    Transform ParseTransform(const nlohmann::json &json) {
        Transform transform;
        if(json.contains("transform") && json["transform"].is_object()) {
            const auto &t = json["transform"];
            SetIfExists(t, "position", transform.position);
            SetIfExists(t, "rotation", transform.rotation);
            SetIfExists(t, "scale", transform.scale);
        }
        return transform;
    }

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
        params.transform = ParseTransform(objJson);
        params.parentName = UNDEFINED_NAME;

        if(objJson.contains("group") && objJson["group"].is_string()) {
            std::string group = objJson["group"].get<std::string>();
            ObjectGroupManager::RegisterGroup(group);
            params.groupCode = ObjectGroupManager::GetGroupCode(group);
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

    void ApplyTransformToGameObject(GameObject* obj, const Transform& transform) {
        if(auto transformComponent = obj->GetComponent<Transform>()) {
            transformComponent->position = transform.position;
            transformComponent->rotation = transform.rotation;
            transformComponent->scale = transform.scale;
        }
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

        ApplyTransformToGameObject(obj, params.transform);
        ApplyGroupToGameObject(obj, objJson);

        if(objJson.contains("components")) {
            CreateComponentsFromJson(obj, objJson["components"]);
        }

        if(!ApplyModelToGameObject(obj, objJson)) {
            return Scene::ABSENT_OBJECT;
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
    }
}
