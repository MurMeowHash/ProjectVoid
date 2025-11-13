#pragma once
#include "../Types/Model/Model.h"
#include "../Types/CreateParameters.h"
#include "../../Renderer/RenderTypes.h"
#include <set>

class GameObject;

namespace Scene {
    const GameObjectParameters DEFAULT_OBJECT_PARAMETERS = GameObjectParameters();
    constexpr LightParameters DEFAULT_LIGHT_PARAMETERS = LightParameters();

    constexpr int ABSENT_OBJECT = -1;
    constexpr uint ABSENT_OBJECT_NUMBER = 0;
    uint CreateGameObject(const GameObjectParameters &params = DEFAULT_OBJECT_PARAMETERS);
    int CreateGameObjectFromModel(Model *model);
    void AddModelMeshesToGameObject(GameObject *obj, Model *model);
    uint CreateCamera(const GameObjectParameters &objParams = DEFAULT_OBJECT_PARAMETERS,
                             const CameraParameters &camParams = CameraParameters());
    uint CreateLight(const GameObjectParameters &objParams = DEFAULT_OBJECT_PARAMETERS,
                     const LightParameters &lightParams = DEFAULT_LIGHT_PARAMETERS);
    void SetObjectName(uint objectIndex, const std::string &newName);
    GameObject *GetGameObjectByIndex(int index);
    NODISCARD int GetGameObjectIndexByName(const std::string &name);
    NODISCARD uint GetLastGameObjectIndex();
    NODISCARD const std::vector<RenderItem3D> &GetGeometryRenderItems();
    NODISCARD const std::vector<GPULight> &GetGPUDirectionalLights();
    NODISCARD const std::vector<GPULight> &GetGPUPointLights();
    NODISCARD const std::vector<GPULight> &GetGPUSpotLights();
    NODISCARD const std::multiset<GPUCamera, GPUCameraComparator> &GetGPUCameras();
    NODISCARD glm::vec3 GetEnvironmentAmbient();
    NODISCARD bool IsInitialized();

    //scene management
    void LoadScene();
    void Start();
    void Update();
    void Dispose();
}