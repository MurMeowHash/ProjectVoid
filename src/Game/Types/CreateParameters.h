#pragma once

#include <glad/glad.h>
#include "../ComponentScripts/Transform/Transform.h"
#include "../../Core/Core.h"
#include "../../Utils/Utils.h"

struct ModelLoadParameters {
    float scaleFactor = 1.0f;
    const char *textureDirectory = "textures/";
};

struct GameObjectParameters {
    std::string name = DEFAULT_OBJECT_NAME;
    Transform transform;
    std::string parentName = UNDEFINED_NAME;
};

enum class ProjectionMode {
    Perspective,
    Orthographic
};

struct PerspectiveParameters {
    float fov = 45.0f;
    float aspectRatio = Core::GetAspectRatio();
};

struct OrthographicParameters {
    float left = -15.0f;
    float right = 15.0f;
    float top = right / Core::GetAspectRatio();
    float bottom = -top;
};

struct CameraParameters {
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    ProjectionMode projectionMode = ProjectionMode::Perspective;
    PerspectiveParameters perspectiveParameters;
    OrthographicParameters orthographicParameters;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    uint cameraPriority = 0;
    int renderTarget = DEFAULT_RENDER_TARGET;
};

enum class TextureWrap {
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER
};

enum class TextureFiltering {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    LinearMipMap = GL_LINEAR_MIPMAP_LINEAR,
    NearestMipMap = GL_NEAREST_MIPMAP_NEAREST
};

enum class BufferFormat {
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    SRGB = GL_SRGB,
    SRGBA = GL_SRGB_ALPHA,
    RGB16F = GL_RGB16F,
    RGBA16F = GL_RGBA16F,
    RGB32F = GL_RGB32F,
    RGBA32F = GL_RGBA32F,
    Depth = GL_DEPTH_COMPONENT,
    DepthStencil = GL_DEPTH24_STENCIL8
};

enum class TextureDataType {
    Float = GL_FLOAT,
    Byte = GL_UNSIGNED_BYTE
};

struct TextureParameters {
    TextureWrap wrapS = TextureWrap::Repeat;
    TextureWrap wrapT = TextureWrap::Repeat;
    TextureFiltering minFilter = TextureFiltering::Nearest;
    TextureFiltering magFilter = TextureFiltering::Nearest;
    BufferFormat desiredFormat = BufferFormat::RGB;
    bool genMipMaps = false;
};

enum class LightType {
    Directional = 0,
    Point = 1,
    Spot = 2
};

struct LightParameters {
    LightType type = LightType::Directional;
    glm::vec3 color = glm::vec3(0.5f);
    float intensity = 1.0f;
    float radius = 5.0f;
    float innerCutOff = 10.0f;
    float outerCutOff = 12.0f;
};