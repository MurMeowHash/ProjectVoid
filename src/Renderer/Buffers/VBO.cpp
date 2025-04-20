#include "VBO.h"

void VBO::Create(GLsizeiptr bufferSize, const void *data) {
    target = GL_ARRAY_BUFFER;
    BufferObject::Create(bufferSize, data);
}

void VBO::SetAttributePointer(GLuint attribute, GLint size, GLenum type, GLsizei stride, GLsizeiptr offset, bool generic, GLuint updateRate) {
    Bind();
    glVertexAttribPointer(attribute, size, type, GL_FALSE, stride, (GLvoid*)(offset));
    if(generic) {
        glEnableVertexAttribArray(attribute);
        glVertexAttribDivisor(attribute, updateRate);
    }
    UnBind();
}