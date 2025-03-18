#include "Scene.h"
#include <map>
#include "../../Debug/Debug.h"
#include <regex>
#include "../../Core/Resources/ResourceManager.h"

namespace Scene {
    std::regex objNamePattern(R"((.*)\s*<(\d+)>$)");

    std::vector<GameObject> gameObjects;
    std::map<std::string, uint> gameObjectIndexMap;
    std::map<std::string, uint> gameObjectNameStat;

    std::string CreateUniqueObjectName(const std::string &name);
    uint ProcessGameObjectNode(MeshNode *node, int parent = ABSENT_OBJECT);

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

    uint ProcessGameObjectNode(MeshNode *node, int parent) {
        GameObjectParameters params = {node->name, node->transform, node->meshes, parent};
        CreateGameObject(params);
        uint currentObject = GetLastGameObjectIndex();

        for(auto child : node->children) {
            ProcessGameObjectNode(child, static_cast<int>(currentObject));
        }

        return currentObject;
    }

    void CreateGameObject(GameObjectParameters &params) {
        params.name = CreateUniqueObjectName(params.name);
        gameObjects.emplace_back(params);
        gameObjectIndexMap[params.name] = gameObjects.size() - 1;
    }

    int CreateGameObjectFromModel(Model *model) {
        if(model == nullptr) {
            Debug::LogError("Scene", "GameObject", "Can not create object from model because the last does not exist");
            return ABSENT_OBJECT;
        }

        return static_cast<int>(ProcessGameObjectNode(model->root));
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

    void LoadScene() {
        ResourceManager::LoadModel("Models/Soldier/soldier.fbx");
        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("soldier")));
    }

    void Update() {
        
    }
}