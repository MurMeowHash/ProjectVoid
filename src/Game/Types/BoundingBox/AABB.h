#pragma once
#include <glm/glm.hpp>
#include <vector>

class AABB {
private:
    glm::vec3 minVertex;
    glm::vec3 maxVertex;
public:
    AABB(const glm::vec3 &minVert, const glm::vec3 &maxVert);
};