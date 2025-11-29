#pragma once
#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class RayCastTest : public ObjectComponent {
DEFINE_BASE(void)
private:
    Camera *cam{nullptr};
public:
    explicit RayCastTest();
    explicit RayCastTest(const RayCastTestParameters& params);

    void Start() override;
    void Update() override;
    
    NODISCARD nlohmann::json SerializeToJson() const override;
    static RayCastTest* CreateFromJson(GameObject* owner, const nlohmann::json& params);
    GET_COMPONENT_TYPE_NAME(RayCastTest)
};
