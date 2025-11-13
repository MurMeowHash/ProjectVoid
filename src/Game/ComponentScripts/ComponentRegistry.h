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
    
private:
    std::unordered_map<std::string, CreateFromJsonFunc> factories;
    std::unordered_map<std::string, SerializeToJsonFunc> serializers;
};

