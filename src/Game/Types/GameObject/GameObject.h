#pragma once
#include "../Transform.h"
#include "../../../Utils/Utils.h"
#include "GameObjectParameters.h"

class GameObject {
private:
    std::string name;
    Transform transform;
    int parent;

    std::vector<uint> meshes;
public:
    explicit GameObject(const GameObjectParameters &params);
    NODISCARD Transform GetTransform() const;
    NODISCARD std::string GetName() const;
    void SetTransform(const Transform &targetTransform);
    void SetPosition(const glm::vec3 &pos);
    void SetRotation(const glm::vec3 &rot);
    void SetScale(const glm::vec3 &scl);
    void SetName(std::string targetName);
};