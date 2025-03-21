#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <assimp/types.h>
#include <vector>

#define NODISCARD [[nodiscard]]
#define UNUSED [[maybe_unused]]

using uint = unsigned int;

constexpr std::string DEFAULT_OBJECT_NAME = "GameObject";
constexpr std::string UNDEFINED_NAME = "Undefined";
constexpr GLsizeiptr MAT4_SIZE = 64;
//TODO: move all constants from resource manager

enum class TextureFormat;

namespace Utils {
    std::string ReadTextFromFile(const std::string &filePath);
    NODISCARD glm::vec3 ToGLMVector(const aiVector3D &target);
    NODISCARD size_t FindLastSlash(const std::string &path);
    NODISCARD std::string GetExtendedNameFromPath(const std::string &path);
    NODISCARD std::string GetNameFromPath(const std::string &path);
    NODISCARD GLenum GetSourceFromDesiredFormat(TextureFormat desiredFormat);
}