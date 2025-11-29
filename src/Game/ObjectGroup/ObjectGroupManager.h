#pragma once
#include <string>
#include "../../Utils/Utils.h"

namespace ObjectGroupManager {
    void Initialize();
    void RegisterGroup(std::string group);
    NODISCARD int GetGroupCode(const std::string &group);
    NODISCARD std::string GetGroupName(int groupCode);
}