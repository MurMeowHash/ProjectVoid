#include "Rigidbody.h"
#include "../../../Engine/Physics/Physics.h"
#include "../../Types/GameObject/GameObject.h"
#include "../Collider/Collider.h"

const RigidbodyParameters Rigidbody::DEFAULT_RB_PARAMS = RigidbodyParameters();

Rigidbody::Rigidbody(const RigidbodyParameters &rbParams)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 1), isKinematic(rbParams.isKinematic), friction(rbParams.friction),
translationConstraints(rbParams.translationConstraints), rotationConstraints(rbParams.rotationConstraints) {
    SetMass(rbParams.mass);
}

void Rigidbody::SetMass(float massValue) {
    mass = std::max(massValue, MIN_MASS);
}

int Rigidbody::GetActiveColliderIndex() const {
    auto owner = GetGameObject();
    if(!owner) return ABSENT_RESOURCE;

    auto collider = owner->GetComponent<Collider>();
    return collider ? collider->GetColliderIndex() : ABSENT_RESOURCE;
}

int Rigidbody::GetRigidbodyIndex() const {
    return rigidbodyIndex;
}

void Rigidbody::UpdateGroup(int groupCode) const {
    Physics::ChangeRigidbodyGroup(rigidbodyIndex, groupCode);
}

void Rigidbody::Start() {
    auto owner = GetGameObject();
    if(!owner) return;

    transform = owner->GetComponent<Transform>();

    auto colliderIndex = GetActiveColliderIndex();
    if(colliderIndex == ABSENT_RESOURCE) {
        Physics::RigidbodyObjectConstructionInfo info(transform->position, transform->rotation, mass,
                                             isKinematic, friction, translationConstraints, rotationConstraints);
        rigidbodyIndex = static_cast<int>(Physics::CreateRigidbody(info, owner->GetGroupCode()));
    }
    else {
        Physics::RigidbodyConstructionInfo info(mass, isKinematic, friction, translationConstraints, rotationConstraints);
        rigidbodyIndex = Physics::CreateRigidbodyFromCollider(info, colliderIndex, owner->GetGroupCode());
        if(rigidbodyIndex == ABSENT_RESOURCE) {
            Debug::LogError("GameObject", owner->GetName(), "Failed to add rigidbody", "Absent collider");
        }
    }
}

void Rigidbody::Update() {
    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    btTransform rbTransform;
    rb->getMotionState()->getWorldTransform(rbTransform);
    *transform = Utils::ToEngineTransform(rbTransform);
}

void Rigidbody::Move(const glm::vec3 &movementVector, const glm::vec3 &rotationVector) const {
    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    if(!rb) return;

    auto rbTransform = Utils::ToBulletTransform(Transform(movementVector, rotationVector));
    rb->setWorldTransform(rbTransform);
    rb->setInterpolationWorldTransform(rbTransform);

    rb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
    rb->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

void Rigidbody::SetLinearVelocity(const glm::vec3 &velocity) const {
    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    if(!rb) return;

    rb->setLinearVelocity(Utils::ToBulletVector(velocity));
}

void Rigidbody::SetAngularVelocity(const glm::vec3 &velocity) const {
    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    if(!rb) return;

    rb->setAngularVelocity(Utils::ToBulletVector(glm::radians(velocity)));
}

void Rigidbody::SetFriction(float frictionValue) {
    friction = frictionValue;
    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    if(!rb) return;

    rb->setFriction(friction);
}

glm::vec3 Rigidbody::GetLinearVelocity() const {
    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    return rb ? Utils::ToGLMVector(rb->getLinearVelocity()) : glm::vec3(0.0f);
}

void Rigidbody::Dispose() {
    Physics::RemoveRigidbody(rigidbodyIndex, GetActiveColliderIndex());
}