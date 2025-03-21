#pragma once
#include "../Transform/Transform.h"
#include "../../../Utils/Utils.h"
#include "../CreateParameters.h"
#include "../../../Renderer/RenderItem3D.h"
#include "../Camera/Camera.h"
#include "../../ComponentScripts/ObjectComponent.h"
#include <unordered_map>
#include <set>
#include <typeindex>
#include "../../../Debug/Debug.h"
#include "../Model/MeshRenderData.h"
#include "../../../Dispose/IDisposable.h"

class GameObject : public IDisposable {
    friend struct Transform;
private:
    std::string name;
    glm::mat4 worldModelMatrix;
    std::string parentName;

    std::unordered_map<std::type_index, ObjectComponent*> components;
    std::multiset<ObjectComponent*, ObjectComponentComparator> prioritizedComponents;

    void UpdatePrioritizedComponents();
public:
    explicit GameObject(const GameObjectParameters &params);
    NODISCARD std::string GetName() const;
    void SetName(std::string targetName);
    void Start();
    void Update();
    void Dispose() override;

    template<typename T, typename... Args> requires std::is_base_of_v<ObjectComponent, T>
    T* AddComponent(Args&& ... args) {
        auto typeInfo = std::type_index(typeid(T));

        if(auto existingComponent = GetComponent<T>(); existingComponent != nullptr) {
            Debug::LogError("GameObject", name, "Such component already exists", typeInfo.name());
            return existingComponent;
        }

        auto newComponent = new T(std::forward<Args>(args)...);
        components[typeInfo] = newComponent;
        newComponent->ownerName = name;
        return newComponent;
    }

    template<typename T> requires (std::is_base_of_v<ObjectComponent, T> && !std::is_same_v<T, Transform>)
    void RemoveComponent() {
        auto componentIterator = components.find(std::type_index(typeid(T)));
        if(componentIterator == components.end()) {
            Debug::LogError("GameObject", name, "Can not remove component", typeid(T).name());
            return;
        }

        componentIterator->second->Dispose();
        delete componentIterator->second;
        components.erase(componentIterator);
    }

    template<typename T> requires std::is_base_of_v<ObjectComponent, T>
    T* GetComponent() const {
        auto componentIterator = components.find(std::type_index(typeid(T)));
        if(componentIterator != components.end()) {
            return static_cast<T*>(componentIterator->second);
        }

        return nullptr;
    }
};