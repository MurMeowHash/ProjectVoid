#pragma once
#include "../../Utils/Utils.h"
#include "../../Game/ComponentScripts/Transform/Transform.h"

namespace Physics {
    void Initialize();
    void Update();
    NODISCARD uint CreateBoxCollider(const Transform &transform);
//    NODISCARD uint CreateCapsuleCollider(btScalar radius, const Transform &transform);
    void Dispose();
}