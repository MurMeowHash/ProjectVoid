#include "GameObject.h"
#include "../../ObjectGroup/ObjectGroupManager.h"
#include "../../ComponentScripts/ComponentRegistry.h"
#include "../../ComponentScripts/Transform/Transform.h"
#include "../../ComponentScripts/Collider/Collider.h"
#include "../../ComponentScripts/Rigidbody/Rigidbody.h"
#include "../../ComponentScripts/MeshRenderData/MeshRenderData.h"
#include "../../../Core/Resources/ResourceManager.h"
#include <nlohmann/json.hpp>
#include <typeindex>
#include <algorithm>

GameObject::GameObject(const GameObjectParameters &params)
: name(params.name), parentName{params.parentName}, tag(params.tag), groupCode(params.groupCode) {
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

std::string GameObject::GetParentName() const {
    return parentName;
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

nlohmann::json GameObject::SerializeToJson() const {
    nlohmann::json objJson;
    objJson["name"] = name;

    auto meshRenderData = GetComponent<MeshRenderData>();
    if(meshRenderData && !meshRenderData->meshes.empty()) {
        std::string modelName = ResourceManager::GetModelNameByMeshes(meshRenderData->meshes);
        if(!modelName.empty()) {
            objJson["model"] = modelName;
        }
    }

    auto transform = GetComponent<Transform>();
    if(transform) {
        nlohmann::json transformJson;
        transformJson["position"] = nlohmann::json::array({transform->position.x, transform->position.y, transform->position.z});
        transformJson["rotation"] = nlohmann::json::array({transform->rotation.x, transform->rotation.y, transform->rotation.z});
        transformJson["scale"] = nlohmann::json::array({transform->scale.x, transform->scale.y, transform->scale.z});
        objJson["transform"] = transformJson;
    }

    if(!parentName.empty() && parentName != UNDEFINED_NAME) {
        objJson["parent"] = parentName;
    }

    std::string groupName = ObjectGroupManager::GetGroupName(groupCode);
    if(!groupName.empty() && groupName != "Default") {
        objJson["group"] = groupName;
    }

    nlohmann::json componentsArray = nlohmann::json::array();
    for(const auto &component: components | std::views::values) {
        if(std::type_index(typeid(*component)) == std::type_index(typeid(Transform))) {
            continue;
        }

        std::string typeName = component->GetComponentTypeName();
        if(typeName.empty() || !ComponentRegistry::Instance().HasSerializer(typeName)) {
            continue;
        }

        nlohmann::json compJson;
        compJson["type"] = typeName;
        compJson["params"] = ComponentRegistry::Instance().Serialize(typeName, component);
        componentsArray.push_back(compJson);
    }

    if(!componentsArray.empty()) {
        objJson["components"] = componentsArray;
    }

    return objJson;
}