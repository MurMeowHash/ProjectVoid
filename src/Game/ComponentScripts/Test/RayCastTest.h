#pragma once
#include "../ObjectComponent.h"

class RayCastTest : public ObjectComponent {
DEFINE_BASE(void)
private:
    Camera *cam{nullptr};
public:
    explicit RayCastTest();

    void Start() override;
    void Update() override;
};