#pragma once
#include "BufferObject.h"

class VBO : public BufferObject {
public:
    void Create(GLsizeiptr bufferSize, const void *data) override;
    void SetAttributePointer(GLuint attribute, GLint size, GLenum type, GLsizei stride, GLsizeiptr offset,
                             bool generic = true, GLuint updateRate = 0);
};