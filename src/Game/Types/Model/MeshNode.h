#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../../../Utils/Utils.h"
#include "../../ComponentScripts/Transform/Transform.h"

struct MeshNode {
    std::string name;
    std::vector<uint> meshes;
    std::vector<MeshNode *> children;
    Transform transform;
};