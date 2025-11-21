#include "Collider.h"
#include "../../Types/GameObject/GameObject.h"
#include "../Rigidbody/Rigidbody.h"
#include "../../../Engine/Physics/Physics.h"
#include <imgui/imgui.h>

Collider::Collider(bool isTrigger)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 0), isTrigger(isTrigger), isEnabled(true) {

}

Collider::Collider(const ColliderParameters &params)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 0), isTrigger(params.isTrigger), isEnabled(params.enabled) {

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
    if(!isEnabled) {
        return;
    }

    auto owner = GetGameObject();
    if(!owner) return;

    ExtractAABB();
    LocateOrigin();
    DetectSize();
}

void Collider::Update() {
    if(!isEnabled || colliderIndex == ABSENT_RESOURCE) {
        return;
    }

    // Оновлюємо колайдер тільки якщо немає Rigidbody (якщо є Rigidbody, він сам оновлює колайдер)
    if(GetActiveRigidbodyIndex() != ABSENT_RESOURCE) {
        return;
    }

    if(colliderIndex == ABSENT_RESOURCE) {
        return;
    }

    auto owner = GetGameObject();
    if(!owner) return;

    auto transform = owner->GetComponent<Transform>();
    if(!transform) return;

    // Оновлюємо origin та створюємо новий Transform для Bullet
    LocateOrigin();
    
    auto collider = Physics::GetColliderByIndex(colliderIndex);
    if(!collider) return;

    // Створюємо Transform з світовою позицією та локальною rotation
    btTransform btTransform = Utils::ToBulletTransform(Transform(origin, transform->rotation));
    collider->setWorldTransform(btTransform);
    collider->setInterpolationWorldTransform(btTransform);
}

void Collider::Dispose() {
    delete aabb;

    Physics::RemoveCollider(colliderIndex, GetActiveRigidbodyIndex());
    colliderIndex = ABSENT_RESOURCE;
}

void Collider::SetEnabled(bool enabled) {
    if(isEnabled == enabled) {
        return;
    }

    isEnabled = enabled;

    if(!isEnabled) {
        if(colliderIndex != ABSENT_RESOURCE) {
            Physics::RemoveCollider(colliderIndex, GetActiveRigidbodyIndex());
            colliderIndex = ABSENT_RESOURCE;
        }
    } else {
        Start();
    }
}

void Collider::RenderUI(GameObject* obj) {
    bool enabled = IsEnabled();
    if(ImGui::Checkbox("Enabled##Collider", &enabled)) {
        SetEnabled(enabled);
    }
    
    ImGui::Spacing();
}