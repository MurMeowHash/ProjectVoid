#include "UBO.h"
#include "../../Debug/Debug.h"
#include <glm/gtc/type_ptr.hpp>

void UBO::Create(GLuint bindingPoint, GLsizeiptr bufferSize, void *data) {
    if(bufferSize > MAX_SIZE) {
        bufferSize = MAX_SIZE;
        Debug::LogWarning("UBO", "Reached a limit of 16 KB");
    }

    target = GL_UNIFORM_BUFFER;
    BufferObject::Create(bufferSize, data);
    BindToPoint(bindingPoint);
}

void UBO::BindToPoint(GLuint bindingPoint) const {
    Bind();
    glBindBufferBase(target, bindingPoint, handle);
    UnBind();
}