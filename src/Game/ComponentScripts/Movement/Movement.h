#pragma once
#include "../ObjectComponent.h"
#include "../Rigidbody/Rigidbody.h"
#include "../ComponentMacros.h"
#include <nlohmann/json_fwd.hpp>

class GameObject;

class Movement : public ObjectComponent {
DEFINE_BASE(void)
private:
    float speed = 10.0f;
    Transform *cameraTransform;
    Rigidbody *rb;
public:
    explicit Movement();
    explicit Movement(const MovementParameters& params);

    void SetCameraTransform(Transform *transform);
    void SetSpeed(float speedValue) { speed = speedValue; }

    void Update() override;
    void Start() override;
    
    NODISCARD nlohmann::json SerializeToJson() const override;
    static Movement* CreateFromJson(GameObject* owner, const nlohmann::json& params);
    GET_COMPONENT_TYPE_NAME(Movement)
};