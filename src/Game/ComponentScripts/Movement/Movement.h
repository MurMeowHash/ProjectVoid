#pragma once
#include "../ObjectComponent.h"

class Movement : public ObjectComponent {
private:
    float speed = 1.0f;
public:
    explicit Movement();
    void Update() override;

};