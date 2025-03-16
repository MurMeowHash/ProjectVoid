#include "TextureParameters.h"

GLenum TextureParameters::GetSourceFromDesiredFormat(TextureFormat desiredFormat) {
    GLenum srcFormat;
    switch (desiredFormat) {
        case TextureFormat::RGB:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        case TextureFormat::SRGB:
            srcFormat = GL_RGB;
            break;
        case TextureFormat::RGBA:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
        case TextureFormat::SRGBA:
            srcFormat = GL_RGBA;
            break;
        case TextureFormat::DepthStencil:
            srcFormat = GL_DEPTH_STENCIL;
            break;
        default:
            srcFormat = static_cast<GLenum>(desiredFormat);
    }

    return srcFormat;
}
