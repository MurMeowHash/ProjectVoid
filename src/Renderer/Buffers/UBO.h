#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../../Utils/Utils.h"
#include "../../Dispose/IDisposable.h"

class UBO : public IDisposable {
private:
    static constexpr GLsizeiptr MAX_SIZE = 16 * 1024;
    GLuint uboHandle = 0;
    GLsizeiptr size = 0;

    void CreateBuffer(void *data);
    void BindBuffer(GLuint bindingPoint) const;
    NODISCARD bool TrackOverflow(GLintptr offset, GLsizeiptr bufSize) const;
public:
    void Create(GLuint bindingPoint, GLsizeiptr bufferSize, void *data = nullptr);
    void SetMat4(GLintptr offset, const glm::mat4 &matrix) const;
    void SetData(GLintptr offset, GLsizeiptr subSize, const void *data) const;

    void Dispose() override;
};