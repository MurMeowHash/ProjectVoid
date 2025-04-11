#pragma once
#include "glad/glad.h"
#include "../GLBuffer.h"
#include "../../CreateParameters.h"

class RenderBuffer : public GLBuffer {
private:
    void CreateBuffer(BufferFormat format);
public:
    RenderBuffer(GLsizei width, GLsizei height, std::string name = UNDEFINED_NAME, BufferFormat format = BufferFormat::Depth);
    void Dispose() override;
};