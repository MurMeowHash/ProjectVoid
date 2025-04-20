#pragma once
#include <glm/glm.hpp>
#include "BufferObject.h"

class UBO : public BufferObject {
private:
    static constexpr GLsizeiptr MAX_SIZE = 16 * 1024;

    void BindToPoint(GLuint bindingPoint) const;
public:
    void Create(GLuint bindingPoint, GLsizeiptr bufferSize, void *data = nullptr);
};