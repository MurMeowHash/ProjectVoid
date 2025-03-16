#pragma once
#include "glad/glad.h"
#include "TextureParameters.h"
#include "../../../Utils/Utils.h"
#include <stb/stb_image.h>

class Texture2D {
private:
    GLuint textureId;
    GLsizei width;
    GLsizei height;
    std::string name;

    void CreateTexture(const TextureParameters &params);
    void InitializeTexture(unsigned char *data, const TextureParameters &params) const;
public:
    Texture2D(std::string name, GLsizei width, GLsizei height,
              const TextureParameters &params = TextureParameters(), unsigned char *data = nullptr);
    NODISCARD std::string GetName() const;
};