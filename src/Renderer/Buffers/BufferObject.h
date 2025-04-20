#pragma once
#include <glad/glad.h>
#include "../../Dispose/IDisposable.h"
#include "../../Utils/Utils.h"

//GL_DYNAMIC_DRAW
class BufferObject : public IDisposable {
protected:
    GLenum target;
    GLuint handle;
    GLsizeiptr size;

    void AllocateBuffer(const void *data);
    NODISCARD bool TrackOverflow(GLintptr offset, GLsizeiptr dataSize) const;
    void Bind() const;
    void UnBind() const;
public:
    virtual void Create(GLsizeiptr bufferSize, const void *data);
    void SetData(GLintptr offset, GLsizeiptr dataSize, const void *data) const;

    void Dispose() override;
};