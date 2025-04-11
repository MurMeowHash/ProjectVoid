#pragma once
#include "glad/glad.h"
#include <string>
#include <vector>
#include "../GLBuffer.h"

struct Attachment {
    GLBuffer *buffer = nullptr;
    bool externalResource = false;
    uint index = 0;
};

enum class BufferType {
    Texture2D,
    RenderBuffer
};

enum class TextureFiltering;

class FrameBuffer : public GLBuffer {
private:
    std::vector<Attachment> colorAttachments;
    Attachment depthAttachment;

    GLBuffer* CreateAttachment(GLenum target, std::string attachmentName, BufferType bufType,
                          BufferFormat bufFormat, TextureFiltering filtering);
    void DisposeDepth();
    void DisposeColor();
public:
    FrameBuffer() = default;
    FrameBuffer(GLsizei width, GLsizei height, std::string name = UNDEFINED_NAME);
    void Create(GLsizei width, GLsizei height, std::string name = UNDEFINED_NAME);
    void CreateColorAttachment(std::string attachmentName, BufferType bufType, BufferFormat bufFormat);
    void CreateDepthAttachment(std::string attachmentName, BufferType bufType, BufferFormat bufFormat);
    void AddColorAttachment(GLBuffer *attachment, BufferType bufType);

    NODISCARD GLuint GetColorAttachment(const std::string &attachName) const;
    NODISCARD GLuint GetDepthAttachment() const;
    NODISCARD const std::vector<Attachment> &GetAllAttachments() const;
    NODISCARD bool IsValid() const;

    void Dispose() override;
};