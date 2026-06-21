#pragma once
#include "../ObjectComponent.h"
#include "../../Types/CreateParameters.h"

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
};
