#pragma once
#include <glad/glad.h>
#include "../Utils/Utils.h"

struct RenderItem3D {
    GLuint vao = 0;
    uint countIndices = 0;
    int albedoMap = -1;
    int normalMap = -1;
    int specularMap = -1;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};