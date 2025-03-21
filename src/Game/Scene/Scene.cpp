#include "Scene.h"
#include <map>
#include "../../Debug/Debug.h"
#include <regex>
#include "../../Core/Resources/ResourceManager.h"
#include "../Types/Camera/Camera.h"
#include "../ComponentScripts/Movement/Movement.h"
#include "../ComponentScripts/Movement/MouseLook.h"

namespace Scene {
    std::regex objNamePattern(R"((.*)\s*<(\d+)>$)");

    std::vector<GameObject> gameObjects;
    std::map<std::string, uint> gameObjectIndexMap;
    std::map<std::string, uint> gameObjectNameStat;

    std::string mainCameraName = UNDEFINED_NAME;

    std::vector<RenderItem3D> geometryRenderItems;

    std::string CreateUniqueObjectName(const std::string &name);
    uint ProcessGameObjectNode(MeshNode *node, const std::string &parentName = UNDEFINED_NAME);
    void UpdateRenderItems();

    std::string CreateUniqueObjectName(const std::string &name) {
        std::smatch patternMatchInfo;
        std::string baseNamePart;
        uint objNumber = ABSENT_OBJECT_NUMBER;
        if(std::regex_match(name, patternMatchInfo, objNamePattern)) {
            baseNamePart = patternMatchInfo[1];
            baseNamePart = baseNamePart.substr(0, baseNamePart.find_last_of(' '));
            objNumber = std::stoi(patternMatchInfo[2]);
        }
        else {
            baseNamePart = name;
        }

        if(GetGameObjectIndexByName(name) == ABSENT_OBJECT) {
            auto nameStatIterator = gameObjectNameStat.find(baseNamePart);
            uint lastNameNumber;
            if(nameStatIterator != gameObjectNameStat.end()) {
                lastNameNumber = std::max(nameStatIterator->second, objNumber);
            } else {
                lastNameNumber = objNumber;
            }

            gameObjectNameStat[baseNamePart] = lastNameNumber;
            return name;
        }

        objNumber = ++gameObjectNameStat[baseNamePart];
        return baseNamePart + " <" + std::to_string(objNumber) + ">";
    }

    uint ProcessGameObjectNode(MeshNode *node, const std::string &parentName) {
        GameObjectParameters params = {node->name, node->transform, parentName};
        CreateGameObject(params);
        uint currentObject = GetLastGameObjectIndex();
        if(!node->meshes.empty()) {
            gameObjects[currentObject].AddComponent<MeshRenderData>(node->meshes);
        }

        for(auto child : node->children) {
            ProcessGameObjectNode(child, gameObjects[currentObject].GetName());
        }

        return currentObject;
    }

    uint CreateGameObject(const GameObjectParameters &params) {
        GameObjectParameters adjustedParameters = params;
        adjustedParameters.name = CreateUniqueObjectName(params.name);
        gameObjects.emplace_back(adjustedParameters);
        gameObjectIndexMap[adjustedParameters.name] = GetLastGameObjectIndex();
        return GetLastGameObjectIndex();
    }

    int CreateGameObjectFromModel(Model *model) {
        if(model == nullptr) {
            Debug::LogError("Scene", "GameObject", "Can not create object from model because the last does not exist");
            return ABSENT_OBJECT;
        }

        return static_cast<int>(ProcessGameObjectNode(model->root));
    }

    uint CreateCamera(const GameObjectParameters &objParams, const CameraParameters &camParams) {
        auto camObjectIndex = CreateGameObject(objParams);
        auto camObject = GetGameObjectByIndex(static_cast<int>(camObjectIndex));
        camObject->AddComponent<Camera>(camParams);
        mainCameraName = camObject->GetName();

        return camObjectIndex;
    }

    void SetMainCamera(const std::string &cameraName) {
        auto cameraObject = GetGameObjectByIndex(GetGameObjectIndexByName(cameraName));
        if(!cameraObject) {
            Debug::LogError("GameObject", cameraName, "Does not exist");
            return;
        }

        auto cameraComponent = cameraObject->GetComponent<Camera>();
        if(!cameraComponent) {
            Debug::LogError("GameObject", cameraObject->GetName(), "Missing component camera");
            return;
        }

        mainCameraName = cameraName;
    }

    void SetObjectName(uint objectIndex, const std::string &newName) {
        auto object = GetGameObjectByIndex(static_cast<int>(objectIndex));
        if(!object) {
            return;
        }

        auto oldName = object->GetName();
        auto uniqueNewName = CreateUniqueObjectName(newName);
        object->SetName(uniqueNewName);
        gameObjectIndexMap.erase(oldName);
        gameObjectIndexMap[uniqueNewName] = objectIndex;
    }

    GameObject *GetGameObjectByIndex(int index) {
        return (index < 0 || index >= gameObjects.size()) ? nullptr : &gameObjects[index];
    }

    int GetGameObjectIndexByName(const std::string &name) {
        auto objIterator = gameObjectIndexMap.find(name);
        if(objIterator == gameObjectIndexMap.end()) {
            return ABSENT_OBJECT;
        }

        return static_cast<int>(objIterator->second);
    }

    uint GetLastGameObjectIndex() {
        return static_cast<uint>(gameObjects.size() - 1);
    }

    const std::vector<RenderItem3D> &GetGeometryRenderItems() {
        return geometryRenderItems;
    }

    const std::string& GetMainCameraName() {
        return mainCameraName;
    }

    void LoadScene() {
        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("soldier")));
        auto mask = GetGameObjectByIndex(GetGameObjectIndexByName("masklow"));
        mask->GetComponent<Transform>()->position /= 100.0f;
        auto body = GetGameObjectByIndex(GetGameObjectIndexByName("body"));
        body->GetComponent<Transform>()->position /= 100.0f;

        GameObjectParameters objParams;

        objParams.name = "Player";
        auto player = GetGameObjectByIndex(CreateCamera(objParams));
        player->AddComponent<Movement>();
        player->AddComponent<MouseLook>();

        //Don't touch
        for(auto &object : gameObjects) {
            object.Start();
        }
    }

    void Update() {
        for(auto &obj : gameObjects) {
            obj.Update();
        }

        UpdateRenderItems();
    }

    void UpdateRenderItems() {
        geometryRenderItems.clear();
        for(const auto &obj : gameObjects) {
            auto objectRenderData = obj.GetComponent<MeshRenderData>();
            if(objectRenderData) {
                geometryRenderItems.insert(geometryRenderItems.end(), objectRenderData->renderItems.begin(),
                                           objectRenderData->renderItems.end());
            }
        }
    }

    void Dispose() {
        for(auto &obj : gameObjects) {
            obj.Dispose();
        }
    }
}