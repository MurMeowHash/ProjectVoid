#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(GLsizei width, GLsizei height, std::string name, BufferFormat format)
: GLBuffer(width, height, std::move(name)) {
    CreateBuffer(format);
}

void RenderBuffer::CreateBuffer(BufferFormat format) {
    glGenRenderbuffers(1, &handle);
    glBindRenderbuffer(GL_RENDERBUFFER, handle);
    glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(format), width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::Dispose() {
    glDeleteRenderbuffers(1, &handle);
}