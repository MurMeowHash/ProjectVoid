#pragma once

#include "../ObjectComponent.h"

class MouseLook : public ObjectComponent {
private:
    float minVerticalAngle = -89.0f;
    float maxVerticalAngle = 89.0f;
    float sensitivityX = 0.1f;
    float sensitivityY = 0.1f;
public:
    explicit MouseLook();
    void Update() override;
};