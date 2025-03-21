#pragma once
#include "glad/glad.h"
#include "../CreateParameters.h"
#include "../../../Utils/Utils.h"
#include <stb/stb_image.h>
#include "../../../Dispose/IDisposable.h"

class Texture2D : public IDisposable {
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
    NODISCARD GLuint GetHandle() const;

    void Dispose() override;
};