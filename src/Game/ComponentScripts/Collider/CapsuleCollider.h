#pragma once
#include "Collider.h"

class CapsuleCollider : public Collider {
DEFINE_BASE(Collider)
private:
    static constexpr float DEFAULT_RADIUS{1.0f};
    static constexpr float DEFAULT_HEIGHT{2.0f};
    float radius = DEFAULT_RADIUS;
    float height = DEFAULT_HEIGHT;

    void DetectSize() override;
public:
    explicit CapsuleCollider(bool isTrigger = false);

    void Start() override;
};