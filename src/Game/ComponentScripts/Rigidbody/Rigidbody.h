#pragma once
#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class Rigidbody : public ObjectComponent {
DEFINE_BASE(void)
private:
    static constexpr float MIN_MASS{10e-6};
    static const RigidbodyParameters DEFAULT_RB_PARAMS;

    int rigidbodyIndex = ABSENT_RESOURCE;
    float mass;
    bool isKinematic;
    float friction;
    glm::bvec3 translationConstraints;
    glm::bvec3 rotationConstraints;

    Transform *transform;

    void SetMass(float massValue);
    NODISCARD int GetActiveColliderIndex() const;
public:
    explicit Rigidbody(const RigidbodyParameters &rbParams = DEFAULT_RB_PARAMS);
    NODISCARD int GetRigidbodyIndex() const;

    void UpdateGroup(int groupCode) const;

    void Move(const glm::vec3 &movementVector = glm::vec3(0.0f), const glm::vec3 &rotationVector = glm::vec3(0.0f)) const;

    void SetLinearVelocity(const glm::vec3 &velocity) const;
    void SetAngularVelocity(const glm::vec3 &velocity) const;
    void SetFriction(float frictionValue);
    NODISCARD glm::vec3 GetLinearVelocity() const;

    void Start() override;
    void Update() override;
    void Dispose() override;
    
    nlohmann::json SerializeToJson() const override;
    GET_COMPONENT_TYPE_NAME(Rigidbody)
    static Rigidbody* CreateFromJson(GameObject* owner, const nlohmann::json& params);
};