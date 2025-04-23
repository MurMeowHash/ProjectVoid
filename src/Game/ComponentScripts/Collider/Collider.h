#pragma once
#include "../ObjectComponent.h"

class Collider : public ObjectComponent {
private:
    int colliderIndex; //?
public:
    Collider();

    void Update() override;
};