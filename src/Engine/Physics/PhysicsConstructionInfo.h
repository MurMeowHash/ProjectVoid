#pragma once
#include <glm/glm.hpp>

namespace Physics {
    struct PhysicsObjectConstructionInfo {
        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
    };

    struct ColliderConstructionInfo : public PhysicsObjectConstructionInfo {
        explicit ColliderConstructionInfo(const glm::vec3 &origin = glm::vec3(0.0f),
                                          const glm::vec3 &rotation = glm::vec3(0.0f),
                                          bool isTrigger = false)
                : PhysicsObjectConstructionInfo(origin, rotation), isTrigger(isTrigger) {}

        bool isTrigger;
    };

    struct BoxColliderConstructionInfo : public ColliderConstructionInfo {
        explicit BoxColliderConstructionInfo(const glm::vec3 &origin = glm::vec3(0.0f),
                                             const glm::vec3 &rotation = glm::vec3(0.0f),
                                             bool isTrigger = false,
                                             const glm::vec3 &size = glm::vec3(1.0f))
                : ColliderConstructionInfo(origin, rotation, isTrigger), size{size} {}
        glm::vec3 size;
    };

    struct CapsuleColliderConstructionInfo : public ColliderConstructionInfo {
        explicit CapsuleColliderConstructionInfo(const glm::vec3 &origin = glm::vec3(0.0f),
                                                 const glm::vec3 &rotation = glm::vec3(0.0f),
                                                 bool isTrigger = false,
                                                 float radius = 1.0f,
                                                 float height = 2.0f)
                : ColliderConstructionInfo(origin, rotation, isTrigger), radius{radius}, height{height} {}
        float radius;
        float height;
    };

    struct RigidbodyConstructionInfo {
        float mass = 1.0f;
        bool isKinematic = false;
        float friction = 1.0f;
        glm::bvec3 translationConstraints = glm::bvec3(false);
        glm::bvec3 rotationConstraints = glm::bvec3(false);
    };

    struct RigidbodyObjectConstructionInfo : public PhysicsObjectConstructionInfo, public RigidbodyConstructionInfo {
        explicit RigidbodyObjectConstructionInfo(const glm::vec3 &origin = glm::vec3(0.0f),
                                                 const glm::vec3 &rotation = glm::vec3(0.0f),
                                                 float mass = 1.0f,
                                                 bool isKinematic = false,
                                                 float friction = 1.0f,
                                                 const glm::bvec3 &translationConstraints = glm::bvec3(false),
                                                 const glm::bvec3 &rotationConstraints = glm::bvec3(false))
                : PhysicsObjectConstructionInfo(origin, rotation),
                  RigidbodyConstructionInfo(mass, isKinematic, friction, translationConstraints, rotationConstraints) {}
    };
}