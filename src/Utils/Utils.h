#pragma once
#include <string>
#include <glm/glm.hpp>
#include <assimp/types.h>
#include <vector>

#define NODISCARD [[nodiscard]]
#define UNUSED [[maybe_unused]]

using uint = unsigned int;

namespace Utils {
    std::string ReadTextFromFile(const std::string &filePath);
    NODISCARD glm::vec3 ToGLMVector(const aiVector3D &target);
    NODISCARD size_t FindLastSlash(const std::string &path);
    NODISCARD std::string GetExtendedNameFromPath(const std::string &path);
    NODISCARD std::string GetNameFromPath(const std::string &path);
}