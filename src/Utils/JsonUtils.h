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
        if (!j[key][0].is_null() && j[key][0].is_number()) {
            outVar.x = j[key][0].get<float>();
        }
        if (!j[key][1].is_null() && j[key][1].is_number()) {
            outVar.y = j[key][1].get<float>();
        }
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::vec3& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 3) {
        if (!j[key][0].is_null() && j[key][0].is_number()) {
            outVar.x = j[key][0].get<float>();
        }
        if (!j[key][1].is_null() && j[key][1].is_number()) {
            outVar.y = j[key][1].get<float>();
        }
        if (!j[key][2].is_null() && j[key][2].is_number()) {
            outVar.z = j[key][2].get<float>();
        }
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::vec4& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 4) {
        if (!j[key][0].is_null() && j[key][0].is_number()) {
            outVar.x = j[key][0].get<float>();
        }
        if (!j[key][1].is_null() && j[key][1].is_number()) {
            outVar.y = j[key][1].get<float>();
        }
        if (!j[key][2].is_null() && j[key][2].is_number()) {
            outVar.z = j[key][2].get<float>();
        }
        if (!j[key][3].is_null() && j[key][3].is_number()) {
            outVar.w = j[key][3].get<float>();
        }
    }
}

template<>
inline void SetIfExists(const nlohmann::json& j, const std::string& key, glm::bvec3& outVar) {
    if (j.contains(key) && j[key].is_array() && j[key].size() == 3) {
        if (!j[key][0].is_null() && j[key][0].is_boolean()) {
            outVar.x = j[key][0].get<bool>();
        }
        if (!j[key][1].is_null() && j[key][1].is_boolean()) {
            outVar.y = j[key][1].get<bool>();
        }
        if (!j[key][2].is_null() && j[key][2].is_boolean()) {
            outVar.z = j[key][2].get<bool>();
        }
    }
}
