#pragma once
#include "../../ComponentScripts/Transform/Transform.h"
#include "../../../Utils/Utils.h"
#include "../CreateParameters.h"
#include "../../../Renderer/RenderTypes.h"
#include "../../ComponentScripts/Camera/Camera.h"
#include "../../ComponentScripts/ObjectComponent.h"
#include <unordered_map>
#include <set>
#include <vector>
#include <typeindex>
#include "../../../Debug/Debug.h"
#include "../../ComponentScripts/MeshRenderData/MeshRenderData.h"
#include "../../../Dispose/IDisposable.h"
#include "../../Scene/Scene.h"

class GameObject : public IDisposable {
    friend struct Transform;
private:
    std::string name;
    std::string parentName;
    std::string tag;
    int groupCode;

    std::unordered_map<int, ObjectComponent*> components;
    std::unordered_map<std::type_index, int> componentsIndexMap;
    std::multiset<ObjectComponent*, ObjectComponentComparator> prioritizedComponents;

    int currentComponentIndex{-1};

    void UpdateColliderGroup() const;
    void UpdateRbGroup() const;

    template<typename T>
    void MapComponent() {
        auto typeIndex = std::type_index(typeid(T));
        auto componentIndexIterator = componentsIndexMap.find(typeIndex);
        if(componentIndexIterator != componentsIndexMap.end()) return;

        if constexpr (IS_COMPONENT(T)) {
            componentsIndexMap[typeIndex] = currentComponentIndex;
            MapComponent<GET_BASE(T)>();
        }
    }

    template<typename T>
    void UnmapComponent() {
        if constexpr (IS_COMPONENT(T)) {
            auto componentIndexMapIterator = componentsIndexMap.find(std::type_index(typeid(T)));
            if(componentIndexMapIterator != componentsIndexMap.end()) {
                componentsIndexMap.erase(componentIndexMapIterator);
                UnmapComponent<GET_BASE(T)>();
            }
        }
    }

    void UpdatePrioritizedComponents();
public:
    explicit GameObject(const GameObjectParameters &params);
    NODISCARD std::string GetName() const;
    NODISCARD int GetGroupCode() const;
    NODISCARD std::string GetTag() const;
    NODISCARD std::string GetParentName() const;
    void SetName(std::string targetName);
    void SetParentName(std::string targetName);
    void SetGroup(const std::string &group);
    void SetTag(std::string tagValue);
    void UpdateComponentsOwnerName(const std::string& newName);
    void Start();
    void Update();
    void Dispose() override;
    
    nlohmann::json SerializeToJson() const;

    template<typename T, typename... Args> requires std::is_base_of_v<ObjectComponent, T>
    T* AddComponent(Args&& ... args) {
        if(auto existingComponent = GetComponent<T>(); existingComponent != nullptr) {
            auto typeInfo = std::type_index(typeid(T));
            Debug::LogError("GameObject", name, "Such component already exists", typeInfo.name());
            return existingComponent;
        }

        auto newComponent = new T(std::forward<Args>(args)...);
        components[++currentComponentIndex] = newComponent;
        MapComponent<T>();
        newComponent->ownerName = name;
        if(Scene::IsInitialized()) {
            newComponent->Start();
        }
        return newComponent;
    }

    template<typename T> requires (std::is_base_of_v<ObjectComponent, T> && !std::is_same_v<T, Transform>)
    void RemoveComponent() {
        auto typeIndex = std::type_index(typeid(T));
        auto componentIndexIterator = componentsIndexMap.find(typeIndex);
        if(componentIndexIterator == componentsIndexMap.end()) {
            Debug::LogError("GameObject", name, "Can not remove component", typeIndex.name());
            return;
        }

        auto componentIterator = components.find(componentIndexIterator->second);
        componentIterator->second->Dispose();
        delete componentIterator->second;
        components.erase(componentIterator);
        UnmapComponent<T>();
    }

    template<typename T> requires std::is_base_of_v<ObjectComponent, T>
    T* GetComponent() const {
        auto componentIndexMapIterator = componentsIndexMap.find(std::type_index(typeid(T)));
        if(componentIndexMapIterator != componentsIndexMap.end()) {
            return static_cast<T*>(components.find(componentIndexMapIterator->second)->second);
        }

        return nullptr;
    }
    
    NODISCARD std::vector<ObjectComponent*> GetAllComponents() const;
};