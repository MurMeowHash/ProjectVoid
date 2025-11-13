#pragma once
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <string>

template<typename T>
void SetIfExists(const nlohmann::json& j, const std::string& key, T& outVar) {
    if (j.contains(key) && !j[key].is_null()) {
        outVar = j.at(key).get<T>();
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::vec2& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 2) {
        outVar.x = j[key][0].get<float>();
        outVar.y = j[key][1].get<float>();
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::vec3& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 3) {
        outVar.x = j[key][0].get<float>();
        outVar.y = j[key][1].get<float>();
        outVar.z = j[key][2].get<float>();
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::vec4& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 4) {
        outVar.x = j[key][0].get<float>();
        outVar.y = j[key][1].get<float>();
        outVar.z = j[key][2].get<float>();
        outVar.w = j[key][3].get<float>();
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::bvec3& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 3) {
        outVar.x = j[key][0].get<bool>();
        outVar.y = j[key][1].get<bool>();
        outVar.z = j[key][2].get<bool>();
    }
}
