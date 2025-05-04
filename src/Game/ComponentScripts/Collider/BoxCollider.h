#pragma once
#include "Collider.h"

class BoxCollider : public Collider {
DEFINE_BASE(Collider)
private:
    static constexpr glm::vec3 DEFAULT_SIZE = glm::vec3(1.0f);
    glm::vec3 size = DEFAULT_SIZE;

    void DetectSize() override;
public:
    explicit BoxCollider(bool isTrigger = false);

    void Start() override;
};