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
#include "../Types/GameObject/GameObject.h"
#include "../ObjectGroup/ObjectGroupManager.h"

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
        //TODO: components must know about new name
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
        auto planeIndex = CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("Plane")));
        auto planeHolder = GetGameObjectByIndex(planeIndex);
        planeHolder->GetComponent<Transform>()->position = glm::vec3(0.0f, -20.0f, 0.0f);
        planeHolder->GetComponent<Transform>()->scale = glm::vec3(100.0f, 0.1f, 100.0f);

//        CreateGameObjectFromModel(ResourceManager::GetModelByIndex(ResourceManager::GetModelIndexByName("Cube")));
//        auto cube = GetGameObjectByIndex(GetGameObjectIndexByName("Cube <1>"));
//        cube->GetComponent<Transform>()->position = glm::vec3(0.0f, 10.0f, 0.0f);
//        cube->GetComponent<Transform>()->rotation = glm::vec3(60.0f, 45.0f, 15.0f);
//        cube->AddComponent<Rigidbody>();

        GameObjectParameters params;
        params.name = "Player";
        ObjectGroupManager::RegisterGroup("Player");
        params.groupCode = ObjectGroupManager::GetGroupCode("Player");
        auto player = GetGameObjectByIndex(static_cast<int>(CreateGameObject(params)));
        player->GetComponent<Transform>()->scale =  glm::vec3(0.5f, 1.8f, 0.5f);
        player->AddComponent<CapsuleCollider>();
        RigidbodyParameters rbParams;
        rbParams.rotationConstraints = glm::bvec3(true, false, true);
        player->AddComponent<Rigidbody>(rbParams);
        player->AddComponent<Movement>();
        params.name = "PlayerCamera";
        auto playerCamera = GetGameObjectByIndex(static_cast<int>(CreateCamera(params)));
        playerCamera->SetParentName("Player");
        playerCamera->GetComponent<Transform>()->position = glm::vec3(0.0f, 1.7f, 0.0f);
        playerCamera->AddComponent<MouseLook>();
        playerCamera->AddComponent<RayCastTest>();
        player->GetComponent<Movement>()->SetCameraTransform(playerCamera->GetComponent<Transform>());

        GameObjectParameters lightObjParams;
        lightObjParams.name = "Light";
        lightObjParams.transform.position = glm::vec3(0.5f, -10.0f, 0.5f);
        lightObjParams.transform.rotation = glm::vec3(-20.0f, 0.0f, 0.0f);
        LightParameters lightParams;
        lightParams.type = LightType::Directional;
        lightParams.color = glm::vec3(0.1f, 0.1f, 0.1f);
        lightParams.intensity = 1000.0f;
        lightParams.radius = 20.0f;
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