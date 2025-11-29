#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <LinearMath/btVector3.h>
#include <LinearMath/btTransform.h>
#include <assimp/types.h>
#include <vector>

#define NODISCARD [[nodiscard]]
#define UNUSED [[maybe_unused]]

using uint = unsigned int;

constexpr std::string DEFAULT_OBJECT_NAME = "GameObject";
constexpr std::string UNDEFINED_NAME = "Undefined";
constexpr std::string DEFAULT_GROUP_NAME = "Default";
constexpr int ABSENT_RESOURCE{-1};
constexpr int DEFAULT_RENDER_TARGET{-1};
constexpr int ALL_GROUPS_CODE{-1};

enum class BufferFormat;
enum class TextureDataType;

class FrameBuffer;
class Transform;
class Camera;

namespace Utils {
    std::string ReadTextFromFile(const std::string &filePath);
    NODISCARD glm::vec3 ToGLMVector(const aiVector3D &target);
    NODISCARD glm::vec3 ToGLMVector(const btVector3 &target);
    NODISCARD btVector3 ToBulletVector(const glm::vec3 &target);
    NODISCARD Transform ToEngineTransform(const btTransform &transform);
    NODISCARD btTransform ToBulletTransform(const Transform &transform);
    NODISCARD size_t FindLastSlash(const std::string &path);
    NODISCARD std::string NormalizePath(const std::string &path);
    NODISCARD int FindOptimalTextLength(const std::string &text, float availableWidth);
    NODISCARD std::string GetExtendedNameFromPath(const std::string &path);
    NODISCARD std::string GetNameFromPath(const std::string &path);
    NODISCARD std::string GetExtensionFromPath(const std::string &path);
    NODISCARD BufferFormat GetSourceFromDesiredFormat(BufferFormat desiredFormat);
    NODISCARD TextureDataType GetTextureDataTypeFromFormat(BufferFormat format);
    NODISCARD GLsizeiptr KBToB(GLsizeiptr kb);
    NODISCARD GLsizeiptr MBToB(GLsizeiptr mb);
    void SaveFrameBufferToJPG(const char *fileName, const FrameBuffer &buf, GLenum colorAttachment, int quality);
    NODISCARD glm::vec3 GetTranslationFromMatrix(const glm::mat4 &matrix);
    NODISCARD glm::vec3 GetScaleFromMatrix(const glm::mat4 &matrix);
    NODISCARD glm::bvec3 InvertVector3(const glm::bvec3 &vec);
    NODISCARD glm::vec3 NDCToWorld(const glm::vec4 &ndcCoord, const Camera &cam);
    void OpenInExplorer(const std::string &path);
    NODISCARD bool IsImageFile(const std::filesystem::path& path);
}