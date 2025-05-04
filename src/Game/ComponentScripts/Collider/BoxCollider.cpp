#include "BoxCollider.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Engine/Physics/Physics.h"

BoxCollider::BoxCollider(bool isTrigger)
: Collider(isTrigger) {

}

void BoxCollider::DetectSize() {
    size = GetGameObject()->GetComponent<Transform>()->GetWorldScale();
    if(aabb) {
        size *= aabb->GetBoxSize();
    }
}

void BoxCollider::Start() {
    Collider::Start();

    auto owner = GetGameObject();
    if(!owner) return;

    Physics::BoxColliderConstructionInfo info(origin, owner->GetComponent<Transform>()->rotation, isTrigger, size);
    colliderIndex = static_cast<int>(Physics::CreateBoxCollider(info, GetActiveRigidbodyIndex(),
                                                                this, owner->GetGroupCode()));
}