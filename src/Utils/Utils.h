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
constexpr int ABSENT_RESOURCE = -1;
constexpr int DEFAULT_RENDER_TARGET = -1;
//TODO: move all constants from resource manager

enum class BufferFormat;
enum class TextureDataType;

namespace Utils {
    std::string ReadTextFromFile(const std::string &filePath);
    NODISCARD glm::vec3 ToGLMVector(const aiVector3D &target);
    NODISCARD size_t FindLastSlash(const std::string &path);
    NODISCARD std::string GetExtendedNameFromPath(const std::string &path);
    NODISCARD std::string GetNameFromPath(const std::string &path);
    NODISCARD std::string GetExtensionFromPath(const std::string &path);
    NODISCARD BufferFormat GetSourceFromDesiredFormat(BufferFormat desiredFormat);
    NODISCARD TextureDataType GetTextureDataTypeFromFormat(BufferFormat format);
}