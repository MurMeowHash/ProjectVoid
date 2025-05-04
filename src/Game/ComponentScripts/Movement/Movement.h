#pragma once
#include "../ObjectComponent.h"
#include "../Rigidbody/Rigidbody.h"

class Movement : public ObjectComponent {
DEFINE_BASE(void)
private:
    float speed = 10.0f;
    Transform *cameraTransform;
    Rigidbody *rb;
public:
    explicit Movement();

    void SetCameraTransform(Transform *transform);

    void Update() override;

    void Start() override;
};