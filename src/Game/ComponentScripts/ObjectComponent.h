#pragma once
#include "../../Utils/Utils.h"
#include "../../Dispose/IDisposable.h"
#include <nlohmann/json.hpp>
#include <string>

#define DEFINE_BASE(base_class) \
public:                         \
using Base = base_class;

#define GET_BASE(class_name) \
typename class_name::Base

#define IS_COMPONENT(class_name) \
!std::is_same_v<class_name, void>

class GameObject;

class ObjectComponent : public IDisposable {
    friend class GameObject;
    friend struct ObjectComponentComparator;
protected:
    static constexpr uint ENGINE_COMPONENTS_START_PRIORITY = 15;
private:
    uint priority;
    std::string ownerName;
public:
    explicit ObjectComponent(uint priority) : priority{priority}, ownerName(UNDEFINED_NAME) {};

    NODISCARD GameObject* GetGameObject() const;

    virtual void Start() {}
    virtual void Update() {}
    void Dispose() override {}
    
    NODISCARD virtual nlohmann::json SerializeToJson() const { return nlohmann::json::object(); }
    NODISCARD virtual std::string GetComponentTypeName() const { return ""; }
};

struct ObjectComponentComparator {
    bool operator()(const ObjectComponent *lhs, const ObjectComponent *rhs) const {
        return lhs->priority < rhs->priority;
    }
};