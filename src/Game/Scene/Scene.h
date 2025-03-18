#pragma once
#include "../Types/GameObject/GameObject.h"
#include "../Types/Model/Model.h"
#include "../Types/GameObject/GameObjectParameters.h"

namespace Scene {
    constexpr int ABSENT_OBJECT = -1;
    constexpr uint ABSENT_OBJECT_NUMBER = 0;
    void CreateGameObject(GameObjectParameters &params);
    int CreateGameObjectFromModel(Model *model);
    GameObject *GetGameObjectByIndex(int index);
    NODISCARD int GetGameObjectIndexByName(const std::string &name);
    NODISCARD uint GetLastGameObjectIndex();

    //scene management
    void LoadScene();
    void Update();
}