#include "SelectionManager.h"

#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"

void SelectionManager::Select(const std::string& name) {
    selectedObjectName = name;
}

void SelectionManager::Clear() {
    selectedObjectName.clear();
}

bool SelectionManager::IsSelected(const std::string& name) const {
    return selectedObjectName == name;
}

bool SelectionManager::HasSelection() const {
    return !selectedObjectName.empty();
}

const std::string& SelectionManager::GetSelected() const {
    return selectedObjectName;
}

void SelectionManager::EnsureValid(const std::vector<GameObjectNode>& hierarchy) {
    if(!selectedObjectName.empty()) {
        int selectedIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
        if(selectedIndex == Scene::ABSENT_OBJECT) {
            selectedObjectName.clear();
        }
    }

    if(selectedObjectName.empty() && !hierarchy.empty()) {
        selectedObjectName = hierarchy[0].name;
    }
}

void SelectionManager::DeleteSelected() {
    if(selectedObjectName.empty()) {
        return;
    }

    int objectIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
    if(objectIndex == Scene::ABSENT_OBJECT) {
        return;
    }

    uint deletedIndex = static_cast<uint>(objectIndex);
    uint lastIndex = Scene::GetLastGameObjectIndex();
    Scene::RemoveGameObject(deletedIndex);

    selectedObjectName.clear();
    if(Scene::GetLastGameObjectIndex() != Scene::ABSENT_OBJECT_NUMBER) {
        if(deletedIndex < lastIndex) {
            auto* nextObj = Scene::GetGameObjectByIndex(static_cast<int>(deletedIndex));
            if(nextObj) {
                selectedObjectName = nextObj->GetName();
            }
        }

        if(selectedObjectName.empty() && deletedIndex > 0) {
            auto* prevObj = Scene::GetGameObjectByIndex(static_cast<int>(deletedIndex - 1));
            if(prevObj) {
                selectedObjectName = prevObj->GetName();
            }
        }
    }
}

