#pragma once
#include <glad/glad.h>
#include <string>
#include "../../../Dispose/IDisposable.h"
#include "../../../Utils/Utils.h"

class GLBuffer : public IDisposable {
protected:
    GLuint handle = GL_NONE;
    GLsizei width = 0;
    GLsizei height = 0;
    std::string name = UNDEFINED_NAME;
public:
    GLBuffer() = default;
    GLBuffer(GLsizei width, GLsizei height, std::string name = UNDEFINED_NAME);
    void CreateBase(GLsizei bufWidth, GLsizei bufHeight, std::string bufName = UNDEFINED_NAME);
    NODISCARD GLuint GetHandle() const;
    NODISCARD GLsizei GetWidth() const;
    NODISCARD GLsizei GetHeight() const;
    NODISCARD std::string GetName() const;
};