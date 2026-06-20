#include "Scene.h"
#include <map>
#include <regex>
#include "../../Core/Resources/ResourceManager.h"
#include "../ComponentScripts/Movement/Movement.h"
#include "../ComponentScripts/Movement/MouseLook.h"
#include "../ComponentScripts/Light/Light.h"
#include "../ComponentScripts/Collider/BoxCollider.h"
#include "../ComponentScripts/Collider/CapsuleCollider.h"
#include "../ComponentScripts/Rigidbody/Rigidbody.h"
#include "../ComponentScripts/Test/RayCastTest.h"
#include "../ComponentScripts/MeshRenderData/MeshRenderData.h"
#include "../Types/GameObject/GameObject.h"
#include "../ObjectGroup/ObjectGroupManager.h"
#include "../../Game/Types/Axis.h"
#include "../ComponentScripts/EditorCamera/EditorMovement.h"
#include "../../Game/ComponentScripts/Camera/Camera.h"

namespace Scene {
    static constexpr float LINEAR_ATT_MULTIPLIER{4.5f};
    static constexpr float QUADRATIC_ATT_MULTIPLIER{44.5f};
    static constexpr float DEFAULT_ATT_COEF{1.0f};

    glm::vec3 environmentAmbient = glm::vec3(0.01f);

    std::regex objNamePattern(R"((.*)\s*<(\d+)>$)");

    std::vector<GameObject> gameObjects;
    std::map<std::string, uint> gameObjectIndexMap;
    std::map<std::string, uint> gameObjectNameStat;

    std::vector<RenderItem3D> geometryRenderItems;
    std::multiset<GPUCamera, GPUCameraComparator> gpuCameras;
    std::vector<GPULight> gpuDirectionalLights;
    std::vector<GPULight> gpuPointLights;
    std::vector<GPULight> gpuSpotLights;

    bool sceneInitialized = false;

    std::string CreateUniqueObjectName(const std::string &name);
    uint ProcessGameObjectNode(MeshNode *node, const std::string &parentName = UNDEFINED_NAME);
    void ExtractRenderItem(const GameObject &obj);
    void ExtractGPULight(const GameObject &obj);
    void ExtractGPUCamera(const GameObject &obj);

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
            gameObjects[currentObject].AddComponent<BoxCollider>();
        }

        for(auto child : node->children) {
            ProcessGameObjectNode(child, gameObjects[currentObject].GetName());
        }

        return currentObject;
    }

    void ExtractRenderItem(const GameObject &obj) {
        auto renderDataComponent = obj.GetComponent<MeshRenderData>();
        if(!renderDataComponent) return;

        geometryRenderItems.insert(geometryRenderItems.end(), renderDataComponent->renderItems.begin(),
                                   renderDataComponent->renderItems.end());
    }

    void ExtractGPULight(const GameObject &obj) {
        auto light = obj.GetComponent<Light>();
        if(!light) return;

        auto transform = obj.GetComponent<Transform>();
        GPULight gpuLight;

        gpuLight.positionType = glm::vec4(transform->position, static_cast<float>(light->type));
        gpuLight.colorIntensity = glm::vec4(light->color, light->intensity);
        gpuLight.directionRadius = glm::vec4(transform->ToForwardVector(), light->radius);

        gpuLight.attenuationCoefs = glm::vec3(DEFAULT_ATT_COEF, LINEAR_ATT_MULTIPLIER / light->radius,
                                              QUADRATIC_ATT_MULTIPLIER / (light->radius * light->radius));
        gpuLight.cutOffs = glm::vec2(glm::cos(glm::radians(light->innerCutOff)),
                                     glm::cos(glm::radians(light->outerCutOff)));

        switch (light->type) {
            case LightType::Directional:
                gpuDirectionalLights.emplace_back(gpuLight);
                break;
            case LightType::Point:
                gpuLight.volumeModelMatrix = glm::translate(gpuLight.volumeModelMatrix, transform->position);
                gpuLight.volumeModelMatrix = glm::scale(gpuLight.volumeModelMatrix, glm::vec3(light->radius));
                gpuPointLights.emplace_back(gpuLight);
                break;
            case LightType::Spot:
                gpuLight.volumeModelMatrix = glm::translate(gpuLight.volumeModelMatrix, transform->position);
                gpuLight.volumeModelMatrix = glm::rotate(gpuLight.volumeModelMatrix,
                                                         glm::radians(transform->rotation.x), Axis::xAxis);
                gpuLight.volumeModelMatrix = glm::rotate(gpuLight.volumeModelMatrix,
                                                         glm::radians(transform->rotation.y), Axis::yAxis);
                gpuLight.volumeModelMatrix = glm::rotate(gpuLight.volumeModelMatrix,
                                                         glm::radians(transform->rotation.z), Axis::zAxis);
                glm::vec3 volumeScale(1.0f);
                volumeScale.y = volumeScale.x = glm::tan(glm::radians(light->outerCutOff)) * light->radius;
                volumeScale.z = light->radius;
                gpuLight.volumeModelMatrix = glm::scale(gpuLight.volumeModelMatrix, volumeScale);
                gpuSpotLights.emplace_back(gpuLight);
                break;
        }
    }

    void ExtractGPUCamera(const GameObject &obj) {
        auto camComponent = obj.GetComponent<Camera>();
        if(!camComponent) return;

        auto camTransform = obj.GetComponent<Transform>();
        gpuCameras.emplace(camTransform->position, camComponent->GetProjectionMatrix(),
                           camComponent->GetViewMatrix(), camComponent->GetRenderTarget(),
                           camComponent->GetCameraPriority());
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
            return ABSENT_RESOURCE;
        }

        return static_cast<int>(ProcessGameObjectNode(model->GetRoot()));
    }

    uint CreateCamera(const GameObjectParameters &objParams, const CameraParameters &camParams) {
        auto camObjectIndex = CreateGameObject(objParams);
        auto camObject = GetGameObjectByIndex(static_cast<int>(camObjectIndex));
        camObject->AddComponent<Camera>(camParams);

        return camObjectIndex;
    }

    uint CreateLight(const GameObjectParameters &objParams, const LightParameters &lightParams) {
        auto lightObjIndex = CreateGameObject(objParams);
        auto lightObject = GetGameObjectByIndex(static_cast<int>(lightObjIndex));
        lightObject->AddComponent<Light>(lightParams);

        return lightObjIndex;
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

    void RemoveGameObject(uint objectIndex) {
        if(gameObjects.empty() || objectIndex >= gameObjects.size()) {
            return;
        }
        
        auto* object = GetGameObjectByIndex(static_cast<int>(objectIndex));
        if(!object) {
            return;
        }

        std::string objectName = object->GetName();

        if(!gameObjects.empty()) {
            for(int i = 0; i <= static_cast<int>(GetLastGameObjectIndex()); ++i) {
                auto* obj = GetGameObjectByIndex(i);
                if(obj && obj->GetParentName() == objectName) {
                    obj->SetParentName(UNDEFINED_NAME);
                }
            }
        }

        object->Dispose();
        gameObjectIndexMap.erase(objectName);
        gameObjects.erase(gameObjects.begin() + objectIndex);

        for(int i = static_cast<int>(objectIndex); i < static_cast<int>(gameObjects.size()); ++i) {
            auto* obj = &gameObjects[i];
            if(obj) {
                gameObjectIndexMap[obj->GetName()] = static_cast<uint>(i);
            }
        }
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

    const std::vector<GPULight> &GetGPUDirectionalLights() {
        return gpuDirectionalLights;
    }

    const std::vector<GPULight> &GetGPUPointLights() {
        return gpuPointLights;
    }

    const std::vector<GPULight> &GetGPUSpotLights() {
        return gpuSpotLights;
    }

    const std::multiset<GPUCamera, GPUCameraComparator> &GetGPUCameras() {
        return gpuCameras;
    }

    glm::vec3 GetEnvironmentAmbient() {
        return environmentAmbient;
    }

    bool IsInitialized() {
        return sceneInitialized;
    }

    void LoadScene() {
        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("XenoRaven")));
        auto xeno = GetGameObjectByIndex(GetGameObjectIndexByName("XenoRaven"));
        xeno->GetComponent<Transform>()->position = glm::vec3(-1.0f, 3.8f, -2.0f);
        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("Mirage_V1")));
        auto v1 = GetGameObjectByIndex(GetGameObjectIndexByName("Mirage_V1"));
        v1->GetComponent<Transform>()->position = glm::vec3(5.0f, -6.5f, 10.0f);
        v1->GetComponent<Transform>()->rotation = glm::vec3(0.0f, 220.0f, 0.0f);
        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("freddy-fazbear-special-delivery")));
        auto freddy = GetGameObjectByIndex(GetGameObjectIndexByName("freddy-fazbear-special-delivery"));
        freddy->GetComponent<Transform>()->position = glm::vec3(0.0f, -6.5f, 10.0f);
        freddy->GetComponent<Transform>()->rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("RockyDesertMesh")));
        auto desert = GetGameObjectByIndex(GetGameObjectIndexByName("RockyDesertMesh"));
        desert->GetComponent<Transform>()->rotation = glm::vec3(270.0f, 0.0f, 0.0f);
        desert->GetComponent<Transform>()->position = glm::vec3(-5.0f, -14.0f, -1.0f);

        GameObjectParameters objParams;

        CameraParameters camParams;
        camParams.cameraPriority = 1;

        GameObjectParameters params;
        params.name = "Player";
        ObjectGroupManager::RegisterGroup("Player");
        params.groupName = ObjectGroupManager::GetGroupCode("Player");
        int playerIndex = static_cast<int>(CreateGameObject(params));
        auto player = GetGameObjectByIndex(playerIndex);
        player->AddComponent<Movement>();
        player->GetComponent<Transform>()->position = glm::vec3(11.6567f, -0.984798f, -5.84777f);
        params.name = "PlayerCamera";
        auto playerCamera = GetGameObjectByIndex(static_cast<int>(CreateCamera(params)));
        playerCamera->SetParentName("Player");
        playerCamera->GetComponent<Transform>()->position = glm::vec3(0.0f, 1.7f, 0.0f);
        playerCamera->GetComponent<Transform>()->rotation = glm::vec3(1.10001f, 281.6f, 0.0f);
        playerCamera->AddComponent<MouseLook>();
        playerCamera->AddComponent<RayCastTest>();
        GetGameObjectByIndex(playerIndex)->GetComponent<Movement>()->SetCameraTransform(playerCamera->GetComponent<Transform>());

        GameObjectParameters lightObjParams;
        lightObjParams.name = "Light";
        lightObjParams.transform.position = glm::vec3(-1.0f, 3.8f, 8.0f);
        lightObjParams.transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
        LightParameters lightParams;
        lightParams.type = LightType::Spot;
        lightParams.color = glm::vec3(0.5f, 0.0f, 0.0f);
        lightParams.radius = 50.0f;
        lightParams.intensity = 5.0f;
        lightParams.innerCutOff = 10.0f;
        lightParams.outerCutOff = 30.0f;
        CreateLight(lightObjParams, lightParams);
        lightObjParams.transform.position = glm::vec3(10.0f, 5.0f, 0.0f);
        lightObjParams.transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        lightParams.type = LightType::Point;
        lightParams.color = glm::vec3(0.0f, 0.0, 0.5f);
        lightParams.radius = 50.0f;
        lightParams.intensity = 2.0f;
        lightParams.innerCutOff = 60.0f;
        lightParams.outerCutOff = 70.0f;
        CreateLight(lightObjParams, lightParams);
        lightObjParams.transform.position = glm::vec3(10.0f, 5.0f, -20.0f);
        lightObjParams.transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        lightParams.type = LightType::Point;
        lightParams.color = glm::vec3(0.5f, 0.0, 0.0f);
        lightParams.radius = 50.0f;
        lightParams.intensity = 2.0f;
        lightParams.innerCutOff = 60.0f;
        lightParams.outerCutOff = 70.0f;
        CreateLight(lightObjParams, lightParams);
        lightObjParams.transform.position = glm::vec3(0.0f, 1.5f, 2.0f);
        lightObjParams.transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        lightParams.type = LightType::Point;
        lightParams.color = glm::vec3(0.2f, 0.2f, 0.5f);
        lightParams.radius = 10.0f;
        lightParams.intensity = 10.0f;
        CreateLight(lightObjParams, lightParams);
        lightObjParams.transform.rotation = glm::vec3(-45.0f, 0.0f, 0.0f);

        lightParams.type = LightType::Directional;
        lightParams.color = glm::vec3(1.0f, 0.875f, 0.43f);
        lightParams.intensity = 0.5f;
        lightObjParams.transform.rotation = glm::vec3(30.0f, 0.0f, 0.0f);
        CreateLight(lightObjParams, lightParams);
    }

    void Start() {
        for(auto &obj : gameObjects) {
            obj.Start();
        }

        sceneInitialized = true;
    }

    void Update() {
        geometryRenderItems.clear();
        gpuDirectionalLights.clear();
        gpuPointLights.clear();
        gpuSpotLights.clear();
        gpuCameras.clear();

        for(auto &obj : gameObjects) {
            obj.Update();

            ExtractRenderItem(obj);
            ExtractGPULight(obj);
            ExtractGPUCamera(obj);
        }
    }

    void Dispose() {
        for(auto &obj : gameObjects) {
            obj.Dispose();
        }
    }
}
