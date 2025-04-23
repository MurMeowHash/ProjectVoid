#include "Physics.h"
#include <btBulletDynamicsCommon.h>
#include "../Time/Time.h"

namespace Physics {
    const btVector3 DEFAULT_GRAVITY = btVector3(0.0f, -9.81f, 0.0f);

    btDynamicsWorld *world{nullptr};
    btDispatcher *dispatcher{nullptr};
    btBroadphaseInterface *broadphase{nullptr};
    btConstraintSolver *constraintSolver{nullptr};
    btCollisionConfiguration *collisionConfig{nullptr};

    std::vector<btCollisionObject *> worldObjects;

    uint CreateCollider(btCollisionShape *colliderShape, const Transform &transform);

    void Initialize() {
        collisionConfig = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfig);
        broadphase = new btDbvtBroadphase();
        constraintSolver = new btSequentialImpulseConstraintSolver();
        world = new btDiscreteDynamicsWorld(dispatcher, broadphase, constraintSolver, collisionConfig);
        world->setGravity(DEFAULT_GRAVITY);
    }

    void Update() {
        world->stepSimulation(Time::GetDeltaTime());
    }

    uint CreateBoxCollider(const Transform &transform) {
        auto colliderShape = new btBoxShape(Utils::ToBulletVector(transform.scale / 2.0f));
        return CreateCollider(colliderShape, transform);
    }

//    uint CreateCapsuleCollider(btScalar radius, btScalar height) {
//        btCapsuleShape()
//    }

    void Dispose() {
        delete collisionConfig;
        delete dispatcher;
        delete broadphase;
        delete constraintSolver;
        delete world;
    }

    uint CreateCollider(btCollisionShape *colliderShape, const Transform &transform) {
        btTransform colliderTransform;
        colliderTransform.setIdentity();
        colliderTransform.setOrigin(Utils::ToBulletVector(transform.position));
        btQuaternion colliderRotation;
        colliderRotation.setEulerZYX(glm::radians(transform.rotation.z), glm::radians(transform.rotation.y),
                                     glm::radians(transform.rotation.x));
        colliderTransform.setRotation(colliderRotation);

        auto collider = new btCollisionObject;
        collider->setCollisionShape(colliderShape);
        collider->setWorldTransform(colliderTransform);
        worldObjects.emplace_back(collider);
        world->addCollisionObject(collider);
        return worldObjects.size() - 1;
    }
}