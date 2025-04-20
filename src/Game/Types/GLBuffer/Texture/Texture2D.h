#pragma once
#include "glad/glad.h"
#include "../GLBuffer.h"
#include "../../CreateParameters.h"

class Texture2D : public GLBuffer {
private:
    void CreateTexture(const TextureParameters &params);
    void InitializeTexture(unsigned char *data, const TextureParameters &params) const;
public:
    Texture2D(GLsizei width, GLsizei height, std::string name = UNDEFINED_NAME,
              const TextureParameters &params = TextureParameters(), unsigned char *data = nullptr);
    void Dispose() override;
};