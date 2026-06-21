#pragma once
#include "../ObjectComponent.h"
#include "../Rigidbody/Rigidbody.h"

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
};