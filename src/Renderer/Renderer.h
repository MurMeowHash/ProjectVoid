#pragma once
#include "RenderData.h"

namespace Renderer {
    void Initialize();
    void RenderFrame(const RenderData &renderData);
    void Dispose();
}