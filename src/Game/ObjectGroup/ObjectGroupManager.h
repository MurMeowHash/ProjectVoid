#pragma once
#include <string>
#include "../../Utils/Utils.h"

namespace ObjectGroupManager {
    void RegisterGroup(std::string group);
    NODISCARD int GetGroupCode(const std::string &group);
}