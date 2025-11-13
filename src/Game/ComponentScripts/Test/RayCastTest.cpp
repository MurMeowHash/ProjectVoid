#include "RayCastTest.h"
#include "../../../Engine/Physics/Physics.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"
#include "../Collider/Collider.h"
#include "../../ObjectGroup/ObjectGroupManager.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

RayCastTest::RayCastTest()
: ObjectComponent(0) {

}

RayCastTest::RayCastTest(const RayCastTestParameters& params)
: ObjectComponent(0) {
}

void RayCastTest::Start() {
    cam = GetGameObject()->GetComponent<Camera>();
}

void RayCastTest::Update() {
    if(Input::GetKeyDown(Input::Key::KeySpace)) {
        auto ray = cam->GetRayFromScreenPoint(Core::GetScreenWidth() / 2, Core::GetScreenHeight() / 2);
        ray.maxDistance = 100.0f;
        int mask = ALL_GROUPS_CODE;
        mask = 1 << ObjectGroupManager::GetGroupCode("Player");
        RayHitInfo hitInfo;
        if(Physics::RayCast(ray, hitInfo, mask)) {
            Debug::Log(hitInfo.collider->GetGameObject()->GetName(), '\n');
            Debug::Log("Hit\n");
            Debug::Log("x: ", hitInfo.surfacePoint.x, " y: ", hitInfo.surfacePoint.y, " z: ", hitInfo.surfacePoint.z, '\n');
        }
    }
}

RayCastTest* RayCastTest::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto rayCastParams = RayCastTestParameters();
    return owner->AddComponent<RayCastTest>(rayCastParams);
}

nlohmann::json RayCastTest::SerializeToJson() const {
    return nlohmann::json::object();
}

REGISTER_COMPONENT_FROM_JSON(RayCastTest)