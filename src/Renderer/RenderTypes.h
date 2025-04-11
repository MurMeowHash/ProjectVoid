#pragma once
#include <glad/glad.h>
#include "../Utils/Utils.h"
#include <glm/glm.hpp>

struct RenderItem3D {
    GLuint vao = 0;
    uint countIndices = 0;
    int albedoMap = ABSENT_RESOURCE;
    int normalMap = ABSENT_RESOURCE;
    int specularMap = ABSENT_RESOURCE;
    float shininess = 0.0f;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

struct GPULight {
    glm::vec4 positionType = glm::vec4(0.0f);
    glm::vec4 colorIntensity = glm::vec4(0.0f);
    glm::vec3 direction = glm::vec3(0.0f);
    alignas(16) glm::vec3 attenuationCoefs = glm::vec3(1.0f);
    alignas(16) glm::vec2 cutOffs = glm::vec2(0.0f);
};

struct GPUCamera {
    glm::vec3 position;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    int renderTarget;
    uint priority = 0;
};

struct GPUCameraComparator {
    bool operator()(const GPUCamera &lhs, const GPUCamera &rhs) const {
        return lhs.priority < rhs.priority;
    }
};

struct PostProcessingInfo {
    float gamma;
    float exposure;
};