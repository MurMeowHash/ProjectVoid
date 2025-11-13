#pragma once
#include "Collider.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class CapsuleCollider : public Collider {
DEFINE_BASE(Collider)
private:
    static constexpr float DEFAULT_RADIUS{1.0f};
    static constexpr float DEFAULT_HEIGHT{2.0f};
    float radius = DEFAULT_RADIUS;
    float height = DEFAULT_HEIGHT;

    void DetectSize() override;
public:
    explicit CapsuleCollider(bool isTrigger = false);
    explicit CapsuleCollider(const ColliderParameters& params);

    void Start() override;
    
    nlohmann::json SerializeToJson() const override;
    GET_COMPONENT_TYPE_NAME(CapsuleCollider)
    static CapsuleCollider* CreateFromJson(GameObject* owner, const nlohmann::json& params);
};