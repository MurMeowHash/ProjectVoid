#pragma once
#include "../Transform.h"
#include <string>

class GameObject;

struct GameObjectParameters {
    std::string name = "Undefined";
    Transform transform = {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)};
    std::vector<uint> meshes = std::vector<uint>();
    int parent = -1;
};