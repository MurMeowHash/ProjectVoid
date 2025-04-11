#pragma once
#include <vector>
#include "RenderTypes.h"
#include <set>

struct RenderData {
    std::vector<RenderItem3D> geometryRenderItems;
    std::multiset<GPUCamera, GPUCameraComparator> gpuCameras;
    std::vector<GPULight> gpuLights;
    glm::vec3 environmentAmbient;
    PostProcessingInfo ppInfo;
};