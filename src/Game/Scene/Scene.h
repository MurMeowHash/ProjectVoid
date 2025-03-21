#pragma once
#include "../Types/GameObject/GameObject.h"
#include "../Types/Model/Model.h"
#include "../Types/CreateParameters.h"

namespace Scene {
    const GameObjectParameters DEFAULT_OBJECT_PARAMETERS = GameObjectParameters();

    constexpr int ABSENT_OBJECT = -1;
    constexpr uint ABSENT_OBJECT_NUMBER = 0;
    uint CreateGameObject(const GameObjectParameters &params = DEFAULT_OBJECT_PARAMETERS);
    int CreateGameObjectFromModel(Model *model);
    uint CreateCamera(const GameObjectParameters &objParams = DEFAULT_OBJECT_PARAMETERS,
                             const CameraParameters &camParams = CameraParameters());
    void SetMainCamera(const std::string &cameraName);
    void SetObjectName(uint objectIndex, const std::string &newName);
    GameObject *GetGameObjectByIndex(int index);
    NODISCARD int GetGameObjectIndexByName(const std::string &name);
    NODISCARD uint GetLastGameObjectIndex();
    NODISCARD const std::vector<RenderItem3D> &GetGeometryRenderItems();
    NODISCARD const std::string& GetMainCameraName();

    //scene management
    void LoadScene();
    void Update();
    void Dispose();
}