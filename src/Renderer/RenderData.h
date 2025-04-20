#pragma once
#include <vector>
#include "RenderTypes.h"
#include <set>

struct RenderData {
    std::vector<RenderItem3D> geometryRenderItems;
    std::multiset<GPUCamera, GPUCameraComparator> gpuCameras;
    std::vector<GPULight> gpuDirectionalLights;
    std::vector<GPULight> gpuPointLights;
    std::vector<GPULight> gpuSpotLights;
    glm::vec3 environmentAmbient;
    PostProcessingInfo ppInfo;
};