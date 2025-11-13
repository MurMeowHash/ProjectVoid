#include "ObjectGroupManager.h"
#include <unordered_map>
#include "../../Debug/Debug.h"

namespace ObjectGroupManager {
    constexpr int MAX_GROUP_INDEX{15};

    std::unordered_map<std::string, int> groups;

    int currentGroupCode{-1};

    inline bool TrackGroupOverflow() {
        return currentGroupCode >= MAX_GROUP_INDEX;
    }

    void RegisterGroup(std::string group) {
        if(GetGroupCode(group) != ALL_GROUPS_CODE) {
            Debug::LogError("ObjectGroupManager", "Such group already exists", group);
            return;
        }

        if(TrackGroupOverflow()) {
            Debug::LogError("ObjectGroupManager", "Limit of groups has been reached", MAX_GROUP_INDEX + 1);
            return;
        }

        groups[std::move(group)] = ++currentGroupCode;
    }

    int GetGroupCode(const std::string &group) {
        auto groupIterator = groups.find(group);
        if(groupIterator == groups.end()) {
            return ALL_GROUPS_CODE;
        }

        return groupIterator->second;
    }

    std::string GetGroupName(int groupCode) {
        if(groupCode == ALL_GROUPS_CODE) {
            return "Default";
        }

        for(const auto& [name, code] : groups) {
            if(code == groupCode) {
                return name;
            }
        }

        return "Default";
    }
}