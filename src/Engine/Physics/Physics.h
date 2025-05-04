#pragma once
#include "../../Utils/Utils.h"
#include "../../Game/ComponentScripts/Transform/Transform.h"
#include "../../Game/Types/BoundingBox/AABB.h"
#include "PhysicsConstructionInfo.h"
#include <btRigidBody.h>
#include "Ray.h"

class Collider;

namespace Physics {
    void Initialize();
    void Update();
    NODISCARD uint CreateBoxCollider(const BoxColliderConstructionInfo &colliderInfo, int rbIndex, Collider *wrap, int groupCode);
    NODISCARD uint CreateCapsuleCollider(const CapsuleColliderConstructionInfo &colliderInfo, int rbIndex, Collider *wrap, int groupCode);
    NODISCARD uint CreateRigidbody(const RigidbodyObjectConstructionInfo &rbInfo, int groupCode);
    NODISCARD int CreateRigidbodyFromCollider(const RigidbodyConstructionInfo &rbInfo, int colliderIndex, int groupCode);
    void RemoveCollider(int colliderIndex, int rbIndex);
    void RemoveRigidbody(int rbIndex, int colliderIndex);
    NODISCARD btRigidBody *GetColliderByIndex(int index);
    NODISCARD btRigidBody *GetRigidbodyByIndex(int index);
    void ChangeColliderGroup(int colliderIndex, int groupCode);
    void ChangeRigidbodyGroup(int rbIndex, int groupCode);

    NODISCARD bool RayCast(const Ray &ray, RayHitInfo &hitInfo, int mask = btBroadphaseProxy::AllFilter);

    void Dispose();
}