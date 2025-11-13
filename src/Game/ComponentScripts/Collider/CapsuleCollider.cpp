#include "CapsuleCollider.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Engine/Physics/Physics.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

CapsuleCollider::CapsuleCollider(bool isTrigger)
: Collider(isTrigger) {

}

CapsuleCollider::CapsuleCollider(const ColliderParameters &params)
: Collider(params) {

}

void CapsuleCollider::DetectSize() {
    auto colliderScale = GetGameObject()->GetComponent<Transform>()->GetWorldScale();
    radius = colliderScale.x;
    height = colliderScale.y;

    if(aabb) {
        auto boundingBoxSize = aabb->GetBoxSize();
        radius *= boundingBoxSize.x;
        height *= boundingBoxSize.y;
    }

    radius /= 2.0f;
}

void CapsuleCollider::Start() {
    Collider::Start();

    auto owner = GetGameObject();
    if(!owner) return;

    Physics::CapsuleColliderConstructionInfo info(origin, owner->GetComponent<Transform>()->rotation, isTrigger, radius, height);
    colliderIndex = static_cast<int>(Physics::CreateCapsuleCollider(info, GetActiveRigidbodyIndex(),
                                                                    this, owner->GetGroupCode()));
}

CapsuleCollider* CapsuleCollider::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto colliderParams = ColliderParameters();
    SetIfExists(params, "isTrigger", colliderParams.isTrigger);
    return owner->AddComponent<CapsuleCollider>(colliderParams);
}

nlohmann::json CapsuleCollider::SerializeToJson() const {
    nlohmann::json params;
    params["isTrigger"] = isTrigger;
    return params;
}

REGISTER_COMPONENT_FROM_JSON(CapsuleCollider)