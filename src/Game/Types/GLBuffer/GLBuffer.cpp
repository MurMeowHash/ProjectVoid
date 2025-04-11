#include "GLBuffer.h"

GLBuffer::GLBuffer(GLsizei width, GLsizei height, std::string name) {
    CreateBase(width, height, std::move(name));
}

void GLBuffer::CreateBase(GLsizei bufWidth, GLsizei bufHeight, std::string bufName) {
    width = bufWidth;
    height = bufHeight;
    name = std::move(bufName);
}

GLuint GLBuffer::GetHandle() const {
    return handle;
}

GLsizei GLBuffer::GetWidth() const {
    return width;
}

GLsizei GLBuffer::GetHeight() const {
    return height;
}

std::string GLBuffer::GetName() const {
    return name;
}