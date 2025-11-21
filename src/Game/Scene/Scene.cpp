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
#include "SceneDeserializer/SceneDeserializer.h"

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
    void CollectMeshesFromNode(MeshNode *node, std::vector<uint> &meshes);
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

        std::vector<uint> allMeshes = node->meshes;

        for(auto child : node->children) {
            CollectMeshesFromNode(child, allMeshes);
        }

        if(!allMeshes.empty()) {
            gameObjects[currentObject].AddComponent<MeshRenderData>(allMeshes);
            gameObjects[currentObject].AddComponent<BoxCollider>();
        }

        return currentObject;
    }
    
    void CollectMeshesFromNode(MeshNode *node, std::vector<uint> &meshes) {
        meshes.insert(meshes.end(), node->meshes.begin(), node->meshes.end());
        for(auto child : node->children) {
            CollectMeshesFromNode(child, meshes);
        }
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

        glm::vec3 lightPosition = (light->type == LightType::Directional) 
            ? transform->position 
            : transform->GetWorldPosition();
        
        gpuLight.positionType = glm::vec4(lightPosition, static_cast<float>(light->type));
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
                gpuLight.volumeModelMatrix = glm::translate(glm::mat4(1.0f), lightPosition);
                gpuLight.volumeModelMatrix = glm::scale(gpuLight.volumeModelMatrix, glm::vec3(light->radius));
                gpuPointLights.emplace_back(gpuLight);
                break;
            case LightType::Spot:
                gpuLight.volumeModelMatrix = glm::translate(glm::mat4(1.0f), lightPosition);
                gpuLight.volumeModelMatrix = glm::rotate(gpuLight.volumeModelMatrix,
                                                         glm::radians(transform->rotation.x), Axis::xAxis);
                gpuLight.volumeModelMatrix = glm::rotate(gpuLight.volumeModelMatrix,
                                                         glm::radians(transform->rotation.y), Axis::yAxis);
                gpuLight.volumeModelMatrix = glm::rotate(gpuLight.volumeModelMatrix,
                                                         glm::radians(transform->rotation.z), Axis::zAxis);
                gpuLight.volumeModelMatrix = glm::scale(gpuLight.volumeModelMatrix, glm::vec3(light->radius));
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

    void AddModelMeshesToGameObject(GameObject *obj, Model *model) {
        if(obj == nullptr || model == nullptr) {
            Debug::LogError("Scene", "AddModelMeshesToGameObject", "Object or model is null");
            return;
        }

        std::vector<uint> allMeshes;
        CollectMeshesFromNode(model->GetRoot(), allMeshes);

        if(!allMeshes.empty()) {
            auto existingMeshData = obj->GetComponent<MeshRenderData>();
            if(existingMeshData) {
                existingMeshData->meshes = allMeshes;
                existingMeshData->modelName = model->GetName();
            } else {
                auto* meshRenderData = obj->AddComponent<MeshRenderData>(allMeshes);
                meshRenderData->modelName = model->GetName();
                if(!obj->GetComponent<BoxCollider>()) {
                    obj->AddComponent<BoxCollider>();
                }
            }
        }
    }

    uint CreateCamera(const GameObjectParameters &objParams, const CameraParameters &camParams) {
        auto camObjectIndex = CreateGameObject(objParams);
        auto camObject = GetGameObjectByIndex(static_cast<int>(camObjectIndex));
        if(camObject) {
            if(!objParams.parentName.empty() && objParams.parentName != UNDEFINED_NAME) {
                camObject->SetParentName(objParams.parentName);
            }
            camObject->AddComponent<Camera>(camParams);
        }

        return camObjectIndex;
    }

    uint CreateLight(const GameObjectParameters &objParams, const LightParameters &lightParams) {
        auto lightObjIndex = CreateGameObject(objParams);
        auto lightObject = GetGameObjectByIndex(static_cast<int>(lightObjIndex));
        if(lightObject) {
            if(!objParams.parentName.empty() && objParams.parentName != UNDEFINED_NAME) {
                lightObject->SetParentName(objParams.parentName);
            }
            lightObject->AddComponent<Light>(lightParams);
        }

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
        object->UpdateComponentsOwnerName(uniqueNewName);
        
        if(!gameObjects.empty()) {
            for(int i = 0; i <= static_cast<int>(GetLastGameObjectIndex()); ++i) {
                auto* obj = GetGameObjectByIndex(i);
                if(obj && obj->GetParentName() == oldName) {
                    obj->SetParentName(uniqueNewName);
                }
            }
        }
        
        if(!object->GetParentName().empty() && object->GetParentName() != UNDEFINED_NAME) {
            auto* transform = object->GetComponent<Transform>();
            if(transform) {
                transform->AdjustToParent();
            }
        }
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
        SceneDeserializer::DeserializeScene(SceneDeserializer::ReadJsonFile("../resources/Scenes/Scene.json"));

        // const auto movement = GetGameObjectByIndex(GetGameObjectIndexByName("Player"))->GetComponent<Movement>();
        // const auto cameraTransform = GetGameObjectByIndex(GetGameObjectIndexByName("PlayerCamera"))->GetComponent<Transform>();
        // movement->SetCameraTransform(cameraTransform);
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
