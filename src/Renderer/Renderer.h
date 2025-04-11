#pragma once
#include "RenderData.h"

class FrameBuffer;

namespace Renderer {
    void Initialize();
    void RenderFrame();
    void Dispose();

    NODISCARD int GetUserFrameBufferByTexture(uint textureIndex);
    NODISCARD FrameBuffer *GetFrameBufferByIndex(int bufIndex);
    NODISCARD int CreateUserFrameBuffer(uint textureIndex);
}