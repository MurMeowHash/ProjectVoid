#include "ObjectComponent.h"
#include "../Scene/Scene.h"

GameObject *ObjectComponent::GetGameObject() const {
    return Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(ownerName));
}