#include "CapsuleCollider.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Engine/Physics/Physics.h"

CapsuleCollider::CapsuleCollider(bool isTrigger)
: Collider(isTrigger) {

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