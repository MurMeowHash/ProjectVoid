#include "GameObject.h"
#include "../../ObjectGroup/ObjectGroupManager.h"
#include "../../ComponentScripts/Collider/Collider.h"
#include "../../ComponentScripts/Rigidbody/Rigidbody.h"

GameObject::GameObject(const GameObjectParameters &params)
: name(params.name), parentName{params.parentName}, tag(params.tag),
groupCode(ObjectGroupManager::GetGroupCode(params.groupName)), activeState(true) {
    AddComponent<Transform>(params.transform);
}

void GameObject::UpdateColliderGroup() const {
    auto collider = GetComponent<Collider>();
    if(!collider) return;

    collider->UpdateGroup(groupCode);
}

void GameObject::UpdateRbGroup() const {
    auto rb = GetComponent<Rigidbody>();
    if(!rb) return;

    rb->UpdateGroup(groupCode);
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

int GameObject::GetGroupCode() const {
    return groupCode;
}

std::string GameObject::GetTag() const {
    return tag;
}

bool GameObject::GetActiveState() const {
    return activeState;
}

void GameObject::SetName(std::string targetName) {
    name = std::move(targetName);
}

void GameObject::SetParentName(std::string targetName) {
    parentName = std::move(targetName);
}

void GameObject::SetGroup(const std::string &group) {
    groupCode = ObjectGroupManager::GetGroupCode(group);
    UpdateColliderGroup();
    UpdateRbGroup();
}

void GameObject::SetTag(std::string tagValue) {
    tag = std::move(tagValue);
}

void GameObject::SetActiveState(bool active) {
    activeState = active;
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