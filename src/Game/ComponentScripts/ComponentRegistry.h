#pragma once
#include <string>
#include <functional>
#include <utility>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "ObjectComponent.h"
#include "../../Debug/Debug.h"

class GameObject;

class ComponentRegistry {
public:
    using CreateFromJsonFunc = std::function<ObjectComponent*(GameObject*, const nlohmann::json&)>;
    using SerializeToJsonFunc = std::function<nlohmann::json(const ObjectComponent*)>;
    using CreateForUIFunc = std::function<void(GameObject*)>;
    using RemoveForUIFunc = std::function<void(GameObject*)>;
    
    struct UIComponentInfo {
        std::string displayName;
        CreateForUIFunc createFunc;
        std::function<bool(GameObject*)> hasComponentFunc;
        RemoveForUIFunc removeFunc;
    };
    
    static ComponentRegistry& Instance() {
        static ComponentRegistry instance;
        return instance;
    }
    
    void Register(const std::string& typeName, CreateFromJsonFunc createFunc) {
        factories[typeName] = std::move(createFunc);
    }
    
    void RegisterSerializer(const std::string& typeName, SerializeToJsonFunc serializeFunc) {
        serializers[typeName] = std::move(serializeFunc);
    }
    
    void RegisterForUI(const std::string& typeName, const std::string& displayName, 
                       CreateForUIFunc createFunc, std::function<bool(GameObject*)> hasComponentFunc,
                       RemoveForUIFunc removeFunc) {
        uiComponents[typeName] = UIComponentInfo{displayName, std::move(createFunc), std::move(hasComponentFunc), std::move(removeFunc)};
    }
    
    ObjectComponent* Create(const std::string& typeName, GameObject* owner, const nlohmann::json& params) {
        const auto it = factories.find(typeName);
        if(it == factories.end()) {
            Debug::LogError("ComponentRegistry", "Unknown component type", typeName);
            return nullptr;
        }
        
        return it->second(owner, params);
    }
    
    nlohmann::json Serialize(const std::string& typeName, const ObjectComponent* component) {
        const auto it = serializers.find(typeName);
        if(it == serializers.end()) {
            Debug::LogError("ComponentRegistry", "Unknown component type for serialization", typeName);
            return nlohmann::json::object();
        }
        
        return it->second(component);
    }
    
    bool HasSerializer(const std::string& typeName) const {
        return serializers.contains(typeName);
    }
    
    const std::unordered_map<std::string, UIComponentInfo>& GetUIComponents() const {
        return uiComponents;
    }
    
private:
    std::unordered_map<std::string, CreateFromJsonFunc> factories;
    std::unordered_map<std::string, SerializeToJsonFunc> serializers;
    std::unordered_map<std::string, UIComponentInfo> uiComponents;
};

