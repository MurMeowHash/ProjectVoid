#pragma once
#include <string>
#include "MeshNode.h"
#include "../../../Utils/Utils.h"

struct Model {
    std::string name;
    MeshNode *root;
};