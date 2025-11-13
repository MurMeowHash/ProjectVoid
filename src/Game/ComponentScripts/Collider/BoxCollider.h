#pragma once
#include "Collider.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class BoxCollider : public Collider {
DEFINE_BASE(Collider)
private:
    static constexpr glm::vec3 DEFAULT_SIZE = glm::vec3(1.0f);
    glm::vec3 size = DEFAULT_SIZE;

    void DetectSize() override;
public:
    explicit BoxCollider(bool isTrigger = false);
    explicit BoxCollider(const ColliderParameters &params);

    void Start() override;
    
    nlohmann::json SerializeToJson() const override;
    GET_COMPONENT_TYPE_NAME(BoxCollider)
    static BoxCollider* CreateFromJson(GameObject* owner, const nlohmann::json& params);
};