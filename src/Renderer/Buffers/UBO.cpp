#include "UBO.h"
#include "../../Debug/Debug.h"
#include <glm/gtc/type_ptr.hpp>

void UBO::Create(GLuint bindingPoint, GLsizeiptr bufferSize, void *data) {
    if(bufferSize > MAX_SIZE) {
        bufferSize = MAX_SIZE;
        Debug::LogWarning("UBO", "Reached a limit of 16 KB");
    }
    size = bufferSize;
    CreateBuffer(data);
    BindBuffer(bindingPoint);
}

void UBO::CreateBuffer(void *data) {
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::BindBuffer(GLuint bindingPoint) const {
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool UBO::TrackOverflow(GLintptr offset, GLsizeiptr bufSize) const {
    if(offset + bufSize > size) {
        Debug::LogError("UBO", "Data overflow", " Buffer size: " + std::to_string(size) + " | Passed data: " + std::to_string(bufSize));
        return true;
    }

    return false;
}

void UBO::SetMat4(GLintptr offset, const glm::mat4 &matrix) const {
    if(TrackOverflow(offset, MAT4_SIZE)) {
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, MAT4_SIZE, glm::value_ptr(matrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::SetData(GLintptr offset, GLsizeiptr subSize, const void *data) const {
    if(TrackOverflow(offset, subSize)) {
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, subSize, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::Dispose() {
    glDeleteBuffers(1, &uboHandle);
}