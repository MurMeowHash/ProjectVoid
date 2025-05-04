#pragma once
#include <glm/glm.hpp>

class Collider;

struct Ray {
    glm::vec3 origin = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f);
    float maxDistance = 0.0f;
};

struct RayHitInfo {
    Collider *collider;
    glm::vec3 surfacePoint;
    glm::vec3 surfaceNormal;
};