#include "AABB.h"

void AABB::DefineBoundingBox(const glm::vec3 &minVert, const glm::vec3 &maxVert) {
    minVertex = minVert;
    maxVertex = maxVert;

    RepositionOrigin();
}

void AABB::RepositionOrigin() {
    origin = (minVertex + maxVertex) / 2.0f;
}

glm::vec3 AABB::GetOrigin() const {
    return origin;
}

glm::vec3 AABB::GetBoxSize() const {
    return maxVertex - minVertex;
}