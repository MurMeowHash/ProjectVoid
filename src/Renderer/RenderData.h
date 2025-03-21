#pragma once
#include "RenderItem3D.h"
#include <vector>

struct CameraData {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
};

struct RenderData {
    std::vector<RenderItem3D> geometryRenderItems;
    CameraData mainCameraData;
};