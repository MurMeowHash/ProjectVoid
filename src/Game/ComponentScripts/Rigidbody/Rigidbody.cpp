#include "Rigidbody.h"
#include "../../../Engine/Physics/Physics.h"
#include "../../Types/GameObject/GameObject.h"
#include "../Collider/Collider.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

const RigidbodyParameters Rigidbody::DEFAULT_RB_PARAMS = RigidbodyParameters();

Rigidbody::Rigidbody(const RigidbodyParameters &rbParams)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 1), isKinematic(rbParams.isKinematic), isEnabled(rbParams.enabled),
friction(rbParams.friction),
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

    if(!isEnabled) {
        return;
    }

    CreatePhysicsBody();
}

void Rigidbody::Update() {
    if(!isEnabled || rigidbodyIndex == ABSENT_RESOURCE || !transform) {
        return;
    }

    auto rb = Physics::GetRigidbodyByIndex(rigidbodyIndex);
    btTransform rbTransform;
    rb->getMotionState()->getWorldTransform(rbTransform);

    glm::vec3 savedScale = transform->scale;
    *transform = Utils::ToEngineTransform(rbTransform);
    transform->scale = savedScale;
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
    rigidbodyIndex = ABSENT_RESOURCE;
}

Rigidbody* Rigidbody::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto rbParams = RigidbodyParameters();
    SetIfExists(params, "mass", rbParams.mass);
    SetIfExists(params, "isKinematic", rbParams.isKinematic);
    SetIfExists(params, "friction", rbParams.friction);
    SetIfExists(params, "translationConstraints", rbParams.translationConstraints);
    SetIfExists(params, "rotationConstraints", rbParams.rotationConstraints);
    SetIfExists(params, "enabled", rbParams.enabled);
    return owner->AddComponent<Rigidbody>(rbParams);
}

nlohmann::json Rigidbody::SerializeToJson() const {
    nlohmann::json params;
    params["mass"] = mass;
    params["isKinematic"] = isKinematic;
    params["friction"] = friction;
    params["translationConstraints"] = nlohmann::json::array({translationConstraints.x, translationConstraints.y, translationConstraints.z});
    params["rotationConstraints"] = nlohmann::json::array({rotationConstraints.x, rotationConstraints.y, rotationConstraints.z});
    params["enabled"] = isEnabled;
    return params;
}

void Rigidbody::CreatePhysicsBody() {
    auto owner = GetGameObject();
    if(!owner || !transform) {
        return;
    }

    if(rigidbodyIndex != ABSENT_RESOURCE) {
        Physics::RemoveRigidbody(rigidbodyIndex, GetActiveColliderIndex());
        rigidbodyIndex = ABSENT_RESOURCE;
    }

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

    if(rigidbodyIndex != ABSENT_RESOURCE) {
        Move(transform->position, transform->rotation);
    }
}

void Rigidbody::SetEnabled(bool enabled) {
    if(isEnabled == enabled) {
        return;
    }

    isEnabled = enabled;

    if(!isEnabled) {
        if(rigidbodyIndex != ABSENT_RESOURCE) {
            Physics::RemoveRigidbody(rigidbodyIndex, GetActiveColliderIndex());
            rigidbodyIndex = ABSENT_RESOURCE;
        }
    } else {
        if(!transform) {
            auto owner = GetGameObject();
            if(owner) {
                transform = owner->GetComponent<Transform>();
            }
        }
        CreatePhysicsBody();
    }
}

REGISTER_COMPONENT_FROM_JSON(Rigidbody)