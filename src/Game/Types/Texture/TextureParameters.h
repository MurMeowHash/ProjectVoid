#pragma once
#include <glad/glad.h>

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

enum class TextureFormat {
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
    TextureFormat desiredFormat = TextureFormat::RGB;
    TextureDataType dataType = TextureDataType::Byte;
    bool genMipMaps = false;

    static GLenum GetSourceFromDesiredFormat(TextureFormat desiredFormat);
};