#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../../../Utils/Utils.h"

class AABB {
private:
    static constexpr glm::vec3 DEFAULT_BOX_COORD = glm::vec3(0.0f);
    glm::vec3 minVertex = DEFAULT_BOX_COORD;
    glm::vec3 maxVertex = DEFAULT_BOX_COORD;
    glm::vec3 origin = DEFAULT_BOX_COORD;

    void RepositionOrigin();
public:
    void DefineBoundingBox(const glm::vec3 &minVert, const glm::vec3 &maxVert);
    NODISCARD glm::vec3 GetOrigin() const;
    NODISCARD glm::vec3 GetBoxSize() const;
};