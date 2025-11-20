#include "BoxCollider.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Engine/Physics/Physics.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

BoxCollider::BoxCollider(bool isTrigger)
: Collider(isTrigger) {

}

BoxCollider::BoxCollider(const ColliderParameters &params)
: Collider(params) {

}


void BoxCollider::DetectSize() {
    size = GetGameObject()->GetComponent<Transform>()->GetWorldScale();
    if(aabb) {
        size *= aabb->GetBoxSize();
    }
}

void BoxCollider::Start() {
    Collider::Start();
    if(!IsEnabled()) {
        return;
    }

    auto owner = GetGameObject();
    if(!owner) return;

    Physics::BoxColliderConstructionInfo info(origin, owner->GetComponent<Transform>()->rotation, isTrigger, size);
    colliderIndex = static_cast<int>(Physics::CreateBoxCollider(info, GetActiveRigidbodyIndex(),
                                                                this, owner->GetGroupCode()));
}

BoxCollider* BoxCollider::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto colliderParams = ColliderParameters();
    SetIfExists(params, "isTrigger", colliderParams.isTrigger);
    SetIfExists(params, "enabled", colliderParams.enabled);
    return owner->AddComponent<BoxCollider>(colliderParams);
}

nlohmann::json BoxCollider::SerializeToJson() const {
    nlohmann::json params;
    params["isTrigger"] = isTrigger;
    params["enabled"] = IsEnabled();
    return params;
}

REGISTER_COMPONENT_FROM_JSON_WITH_UI(BoxCollider, "Box Collider")