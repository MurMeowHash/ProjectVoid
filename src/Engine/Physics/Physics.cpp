#include "Physics.h"
#include <btBulletDynamicsCommon.h>
#include "../Time/Time.h"
#include <map>
#include "../../Game/ComponentScripts/Collider/Collider.h"

namespace Physics {
    const btVector3 DEFAULT_GRAVITY = btVector3(0.0f, -9.81f, 0.0f);
    constexpr float ABSENT_MASS{0.0f};

    btDynamicsWorld *world{nullptr};
    btDispatcher *dispatcher{nullptr};
    btBroadphaseInterface *broadphase{nullptr};
    btConstraintSolver *constraintSolver{nullptr};
    btCollisionConfiguration *collisionConfig{nullptr};

    std::map<int, btRigidBody *> colliders;
    std::map<int, btRigidBody *> rigidbodies;

    int colliderCurrentIndex{-1};
    int rigidbodyCurrentIndex{-1};

    btCollisionShape *emptyShape{nullptr};

    btVector3 GetLocalInertia(float mass, btCollisionShape *shape);
    btRigidBody *CreatePhysicsObject(float mass, btCollisionShape *collisionShape, const PhysicsObjectConstructionInfo &objInfo);
    btRigidBody *CreatePhysicsObject(float mass, btCollisionShape *collisionShape,
                                     btMotionState *motionState, void *wrap, int collisionFlags, int flags);
    uint CreateCollider(btCollisionShape *collisionShape, const ColliderConstructionInfo &colliderInfo,
                        int rbIndex, Collider *wrap, int groupCode);
    void ChangeRigidbodyShape(btRigidBody *rb, btCollisionShape *collisionShape);
    void SetUpRigidbody(btRigidBody *rb, const RigidbodyConstructionInfo &rbInfo);
    uint RegisterObject(btRigidBody *object, int &currentIndex, std::map<int, btRigidBody *> &objectHolder);
    void AddObjectToPhysicsWorld(btRigidBody *object, int group);
    uint AddObjectToScene(btRigidBody *object, int &currentIndex, std::map<int, btRigidBody *> &objectHolder, int groupCode);
    btRigidBody *GetObjectByIndex(int index, std::map<int, btRigidBody *> &objectHolder);
    NODISCARD int CodeGroup(int groupCode);
    void ChangeObjectGroup(int objIndex, int groupCode, const std::function<btRigidBody*(int)> &objGetter);

    void Initialize() {
        collisionConfig = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfig);
        broadphase = new btDbvtBroadphase();
        constraintSolver = new btSequentialImpulseConstraintSolver();
        world = new btDiscreteDynamicsWorld(dispatcher, broadphase, constraintSolver, collisionConfig);
        world->setGravity(DEFAULT_GRAVITY);

        emptyShape = new btEmptyShape;
    }

    void Update() {
        world->stepSimulation(Time::GetDeltaTime());
    }

    uint CreateBoxCollider(const BoxColliderConstructionInfo &colliderInfo, int rbIndex, Collider *wrap, int groupCode) {
        auto boxShape = new btBoxShape(Utils::ToBulletVector(colliderInfo.size / 2.0f));
        return CreateCollider(boxShape, colliderInfo, rbIndex, wrap, groupCode);
    }

    uint CreateCapsuleCollider(const CapsuleColliderConstructionInfo &colliderInfo, int rbIndex, Collider *wrap, int groupCode) {
        auto capsuleShape = new btCapsuleShape(colliderInfo.radius, colliderInfo.height);
        return CreateCollider(capsuleShape, colliderInfo, rbIndex, wrap, groupCode);
    }

    uint CreateRigidbody(const RigidbodyObjectConstructionInfo &rbInfo, int groupCode) {
        auto rb = CreatePhysicsObject(rbInfo.isKinematic ? ABSENT_MASS : rbInfo.mass, emptyShape, rbInfo);
        SetUpRigidbody(rb, rbInfo);
        return AddObjectToScene(rb, rigidbodyCurrentIndex, rigidbodies, groupCode);
    }

    int CreateRigidbodyFromCollider(const RigidbodyConstructionInfo &rbInfo, int colliderIndex, int groupCode) {
        auto collider = GetColliderByIndex(colliderIndex);
        if(!collider) {
            return ABSENT_RESOURCE;
        }

        auto motionState = collider->getMotionState();
        auto collisionShape = collider->getCollisionShape();
        auto colliderWrap = collider->getUserPointer();
        auto flags = collider->getFlags();
        auto rb = CreatePhysicsObject(rbInfo.isKinematic ? ABSENT_MASS : rbInfo.mass, collisionShape,
                                      motionState, colliderWrap, 0, flags);
        SetUpRigidbody(rb, rbInfo);

        world->removeRigidBody(collider);
        return static_cast<int>(AddObjectToScene(rb, rigidbodyCurrentIndex, rigidbodies, groupCode));
    }

    void RemoveCollider(int colliderIndex, int rbIndex) {
        auto collider = GetColliderByIndex(colliderIndex);
        if(!collider) return;

        auto rb = GetRigidbodyByIndex(rbIndex);
        if(rb) {
            ChangeRigidbodyShape(rb, emptyShape);
        } else {
            world->removeRigidBody(collider);
            delete collider->getMotionState();
        }

        delete collider->getCollisionShape();
        delete collider;
        colliders.erase(colliderIndex);
    }

    void RemoveRigidbody(int rbIndex, int colliderIndex) {
        auto rb = GetRigidbodyByIndex(rbIndex);
        if(!rb) return;

        auto rbGroup = rb->getBroadphaseProxy()->m_collisionFilterGroup;
        world->removeRigidBody(rb);
        auto collider = GetColliderByIndex(colliderIndex);
        if(collider) {
            AddObjectToPhysicsWorld(collider, rbGroup);
        }
        else {
            delete rb->getMotionState();
        }
        delete rb;
        rigidbodies.erase(rbIndex);
    }

    btRigidBody *GetColliderByIndex(int index) {
        return GetObjectByIndex(index, colliders);
    }

    btRigidBody *GetRigidbodyByIndex(int index) {
        return GetObjectByIndex(index, rigidbodies);
    }

    void ChangeColliderGroup(int colliderIndex, int groupCode) {
        ChangeObjectGroup(colliderIndex, groupCode, GetColliderByIndex);
    }

    void ChangeRigidbodyGroup(int rbIndex, int groupCode) {
        ChangeObjectGroup(rbIndex, groupCode, GetRigidbodyByIndex);
    }

    bool RayCast(const Ray &ray, RayHitInfo &hitInfo, int mask) {
        auto rayFromWorld = Utils::ToBulletVector(ray.origin);
        auto rayToWorld = Utils::ToBulletVector(ray.origin + ray.direction * ray.maxDistance);
        btCollisionWorld::ClosestRayResultCallback rayCastCallback(rayFromWorld, rayToWorld);
        rayCastCallback.m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
        rayCastCallback.m_collisionFilterMask = mask;
        world->rayTest(rayFromWorld, rayToWorld, rayCastCallback);
        if(!rayCastCallback.hasHit()) return false;

        hitInfo.collider = static_cast<Collider*>(rayCastCallback.m_collisionObject->getUserPointer());
        hitInfo.surfacePoint = Utils::ToGLMVector(rayCastCallback.m_hitPointWorld);
        hitInfo.surfaceNormal = Utils::ToGLMVector(rayCastCallback.m_hitNormalWorld);
        return true;
    }

    void Dispose() {
        delete emptyShape;
        delete world;
        delete collisionConfig;
        delete dispatcher;
        delete broadphase;
        delete constraintSolver;
    }

    btVector3 GetLocalInertia(float mass, btCollisionShape *shape) {
        btVector3 localInertia(0.0f, 0.0f, 0.0f);
        if(mass != ABSENT_MASS) {
            shape->calculateLocalInertia(mass, localInertia);
        }

        return localInertia;
    }

    btRigidBody *CreatePhysicsObject(float mass, btCollisionShape *collisionShape, const PhysicsObjectConstructionInfo &objInfo) {
        auto transform = Utils::ToBulletTransform(Transform(objInfo.origin, objInfo.rotation));
        auto motionState = new btDefaultMotionState(transform);
        btVector3 localInertia = GetLocalInertia(mass, collisionShape);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
        return new btRigidBody(rbInfo);
    }

    btRigidBody *CreatePhysicsObject(float mass, btCollisionShape *collisionShape,
                                     btMotionState *motionState, void *wrap, int collisionFlags, int flags) {
        btVector3 localInertia = GetLocalInertia(mass, collisionShape);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
        auto rb = new btRigidBody(rbInfo);
        rb->setCollisionFlags(rb->getCollisionFlags() | collisionFlags);
        rb->setFlags(rb->getFlags() | flags);
        rb->setUserPointer(wrap);
        return rb;
    }

    uint CreateCollider(btCollisionShape *collisionShape, const ColliderConstructionInfo &colliderInfo,
                        int rbIndex, Collider *wrap, int groupCode) {
        btRigidBody *collider{nullptr};
        auto rb = GetRigidbodyByIndex(rbIndex);
        if(!rb) {
            collider = CreatePhysicsObject(ABSENT_MASS, collisionShape, colliderInfo);
        }
        else {
            auto motionState = rb->getMotionState();
            auto flags = rb->getFlags();
            collider = CreatePhysicsObject(ABSENT_MASS, collisionShape, motionState, nullptr, 0, flags);
            ChangeRigidbodyShape(rb, collider->getCollisionShape());
            rb->setUserPointer(wrap);
        }

        if(colliderInfo.isTrigger) {
            collider->setFlags(collider->getFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        collider->setUserPointer(wrap);

        return rb ? RegisterObject(collider, colliderCurrentIndex, colliders)
                    : AddObjectToScene(collider, colliderCurrentIndex, colliders, groupCode);
    }

    void ChangeRigidbodyShape(btRigidBody *rb, btCollisionShape *collisionShape) {
        world->removeRigidBody(rb);
        auto localInertia = GetLocalInertia(rb->getMass(), collisionShape);
        rb->setMassProps(rb->getMass(), localInertia);
        rb->setCollisionShape(collisionShape);
        rb->updateInertiaTensor();
        world->addRigidBody(rb);
    }

    void SetUpRigidbody(btRigidBody *rb, const RigidbodyConstructionInfo &rbInfo) {
        if(rbInfo.isKinematic) {
            rb->setCollisionFlags(rb->getCollisionFlags() | btRigidBody::CF_KINEMATIC_OBJECT);
        }

        glm::vec3 translationFactor = Utils::InvertVector3(rbInfo.translationConstraints);
        glm::vec3 rotationFactor = Utils::InvertVector3(rbInfo.rotationConstraints);
        rb->setLinearFactor(Utils::ToBulletVector(translationFactor));
        rb->setAngularFactor(Utils::ToBulletVector(rotationFactor));
        rb->setFriction(rbInfo.friction);

        rb->setActivationState(DISABLE_DEACTIVATION);
    }

    uint RegisterObject(btRigidBody *object, int &currentIndex, std::map<int, btRigidBody *> &objectHolder) {
        objectHolder[++currentIndex] = object;
        return currentIndex;
    }

    void AddObjectToPhysicsWorld(btRigidBody *object, int group) {
        world->addRigidBody(object, group, btBroadphaseProxy::AllFilter);
    }

    uint AddObjectToScene(btRigidBody *object, int &currentIndex, std::map<int, btRigidBody *> &objectHolder, int groupCode) {
        AddObjectToPhysicsWorld(object, CodeGroup(groupCode));
        return RegisterObject(object, currentIndex, objectHolder);
    }

    btRigidBody *GetObjectByIndex(int index, std::map<int, btRigidBody *> &objectHolder) {
        auto object = objectHolder.find(index);
        if(object == objectHolder.end()) {
            return nullptr;
        }

        return object->second;
    }

    int CodeGroup(int groupCode) {
        return groupCode == ALL_GROUPS_CODE ? btBroadphaseProxy::AllFilter : 1 << groupCode;
    }

    void ChangeObjectGroup(int objIndex, int groupCode, const std::function<btRigidBody*(int)> &objGetter) {
        auto obj = objGetter(objIndex);
        if(!obj) return;

        auto objBroadphaseProxy = obj->getBroadphaseProxy();
        if(!objBroadphaseProxy) return;

        world->removeRigidBody(obj);
        AddObjectToPhysicsWorld(obj, CodeGroup(groupCode));
    }
}