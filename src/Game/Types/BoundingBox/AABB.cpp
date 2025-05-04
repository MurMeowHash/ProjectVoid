#include "AABB.h"

void AABB::DefineBoundingBox(const glm::vec3 &minVert, const glm::vec3 &maxVert) {
    minVertex = minVert;
    maxVertex = maxVert;

    RepositionOrigin();
}

AABB::AABB(const glm::vec3 &minVert, const glm::vec3 &maxVert) {
    DefineBoundingBox(minVert, maxVert);
}

void AABB::RepositionOrigin() {
    origin = (minVertex + maxVertex) / 2.0f;
}

glm::vec3 AABB::GetOrigin() const {
    return origin;
}

glm::vec3 AABB::GetBoxSize() const {
    return glm::max(maxVertex - minVertex, MIN_BOX_SIZE);
}

glm::vec3 AABB::GetMinVertex() const {
    return minVertex;
}

glm::vec3 AABB::GetMaxVertex() const {
    return maxVertex;
}