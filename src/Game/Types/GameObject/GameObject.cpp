#include "GameObject.h"
#include "../../Scene/Scene.h"

GameObject::GameObject(const GameObjectParameters &params)
: name(params.name), parentName{params.parentName} {
    AddComponent<Transform>(params.transform);
}

void GameObject::UpdatePrioritizedComponents() {
    prioritizedComponents.clear();
    for(const auto &component : components) {
        prioritizedComponents.insert(component.second);
    }
}


std::string GameObject::GetName() const {
    return name;
}

void GameObject::SetName(std::string targetName) {
    name = std::move(targetName);
}

void GameObject::Start() {
    UpdatePrioritizedComponents();

    for(auto &component : prioritizedComponents) {
        component->Start();
    }
}

void GameObject::Update() {
    UpdatePrioritizedComponents();

    for(auto &component : prioritizedComponents) {
        component->Update();
    }
}

void GameObject::Dispose() {
    for(auto &component : components) {
        component.second->Dispose();
        delete component.second;
    }
}
