#pragma once
#include "../../Utils/Utils.h"
#include "../../Dispose/IDisposable.h"

class GameObject;

class ObjectComponent : public IDisposable {
    friend class GameObject;
    friend struct ObjectComponentComparator;
private:
    uint priority;
    std::string ownerName;
protected:
    NODISCARD GameObject* GetGameObject() const;
public:
    explicit ObjectComponent(uint priority) : priority{priority}, ownerName(UNDEFINED_NAME) {};

    virtual void Start() {}

    virtual void Update() {}

    void Dispose() override {}
};

struct ObjectComponentComparator {
    bool operator()(const ObjectComponent *lhs, const ObjectComponent *rhs) const {
        return lhs->priority < rhs->priority;
    }
};