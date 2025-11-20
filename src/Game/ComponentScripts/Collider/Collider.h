#pragma once
#include "../ObjectComponent.h"
#include "../../Types/BoundingBox/AABB.h"
#include <functional>

struct ColliderParameters {
    bool isTrigger = false;
    bool enabled = true;
};

class Rigidbody;

class Collider : public ObjectComponent {
DEFINE_BASE(void)
private:
    static constexpr glm::vec3 DEFAULT_ORIGIN = glm::vec3(0.0f);
    void ExtractAABB();
    void LocateOrigin();
protected:
    glm::vec3 origin = DEFAULT_ORIGIN;
    bool isTrigger;
    bool isEnabled = true;
    int colliderIndex = ABSENT_RESOURCE;
    AABB *aabb = nullptr;
    NODISCARD int GetActiveRigidbodyIndex() const;

    virtual void DetectSize() = 0;
public:
    explicit Collider(bool isTrigger = false);
    explicit Collider(const ColliderParameters& params);
    NODISCARD int GetColliderIndex() const;

    void UpdateGroup(int groupCode) const;

    void Start() override;
    void Update() override;
    void Dispose() override;

    void SetEnabled(bool enabled);
    NODISCARD bool IsEnabled() const { return isEnabled; }
};