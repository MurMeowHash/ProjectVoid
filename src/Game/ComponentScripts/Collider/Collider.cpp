#include "Collider.h"
#include "../../Types/GameObject/GameObject.h"
#include "../Rigidbody/Rigidbody.h"
#include "../../../Engine/Physics/Physics.h"

Collider::Collider(bool isTrigger)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 0), isTrigger(isTrigger) {

}

void Collider::ExtractAABB() {
    delete aabb;

    auto meshData = GetGameObject()->GetComponent<MeshRenderData>();
    if(meshData) {
        aabb = new AABB(meshData->GetCommonAABB());
    }
}

void Collider::LocateOrigin() {
    origin = GetGameObject()->GetComponent<Transform>()->GetWorldPosition();

    if(aabb) {
        origin += aabb->GetOrigin();
    }
}

int Collider::GetActiveRigidbodyIndex() const {
    auto owner = GetGameObject();
    if(!owner) return ABSENT_RESOURCE;

    auto rb = owner->GetComponent<Rigidbody>();
    return rb ? rb->GetRigidbodyIndex() : ABSENT_RESOURCE;
}

int Collider::GetColliderIndex() const {
    return colliderIndex;
}

void Collider::UpdateGroup(int groupCode) const {
    Physics::ChangeColliderGroup(colliderIndex, groupCode);
}

void Collider::Start() {
    auto owner = GetGameObject();
    if(!owner) return;

    ExtractAABB();
    LocateOrigin();
    DetectSize();
}

void Collider::Dispose() {
    delete aabb;

    Physics::RemoveCollider(colliderIndex, GetActiveRigidbodyIndex());
}