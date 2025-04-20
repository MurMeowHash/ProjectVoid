#include "BufferObject.h"
#include "../../Debug/Debug.h"
#include <glm/gtc/type_ptr.hpp>

void BufferObject::Create(GLsizeiptr bufferSize, const void *data) {
    size = bufferSize;
    AllocateBuffer(data);
}

void BufferObject::AllocateBuffer(const void *data) {
    glGenBuffers(1, &handle);
    glBindBuffer(target, handle);
    glBufferData(target, size, data, GL_STATIC_DRAW);
    glBindBuffer(target, 0);
}

bool BufferObject::TrackOverflow(GLintptr offset, GLsizeiptr dataSize) const {
    if(offset + dataSize > size) {
        Debug::LogError("Buffer", target, "Data overflow",
                        "Buffer size: " + std::to_string(size) + " | Passed data: " + std::to_string(dataSize));
        return true;
    }

    return false;
}

void BufferObject::Bind() const {
    glBindBuffer(target, handle);
}

void BufferObject::UnBind() const {
    glBindBuffer(target, 0);
}

void BufferObject::SetData(GLintptr offset, GLsizeiptr dataSize, const void *data) const {
    if(TrackOverflow(offset, dataSize)) {
        return;
    }

    glBindBuffer(target, handle);
    auto buffer = glMapBufferRange(target, offset, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    std::memcpy(buffer, data, dataSize);
    glUnmapBuffer(target);
    glBindBuffer(target, 0);
}

void BufferObject::Dispose() {
    glDeleteBuffers(1, &handle);
}