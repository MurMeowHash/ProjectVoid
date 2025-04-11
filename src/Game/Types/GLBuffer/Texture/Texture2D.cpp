#include "Texture2D.h"
#include "stb/stb_image.h"
#include "../../../../Debug/Debug.h"

Texture2D::Texture2D(GLsizei width, GLsizei height, std::string name,
                     const TextureParameters &params, unsigned char *data)
: GLBuffer(width, height, std::move(name)) {
    CreateTexture(params);
    InitializeTexture(data, params);
}

void Texture2D::CreateTexture(const TextureParameters &params) {
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(params.wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(params.wrapT));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(params.minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(params.magFilter));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::InitializeTexture(unsigned char *data, const TextureParameters &params) const {
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(params.desiredFormat), width, height, 0,
                 static_cast<GLenum>(Utils::GetSourceFromDesiredFormat(params.desiredFormat)),
                 static_cast<GLenum>(Utils::GetTextureDataTypeFromFormat(params.desiredFormat)), data);
    if(params.genMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Dispose() {
    glDeleteTextures(1, &handle);
}