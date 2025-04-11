#include "FrameBuffer.h"
#include "../../CreateParameters.h"
#include "../Texture/Texture2D.h"
#include "../RenderBuffer/RenderBuffer.h"
#include "../../../../Debug/Debug.h"

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, std::string name)
: GLBuffer(width, height, std::move(name)) {
    glGenFramebuffers(1, &handle);
}

void FrameBuffer::Create(GLsizei width, GLsizei height, std::string name) {
    CreateBase(width, height, std::move(name));
    glGenFramebuffers(1, &handle);
}

GLBuffer* FrameBuffer::CreateAttachment(GLenum target, std::string attachmentName, BufferType bufType, BufferFormat bufFormat,
                                   TextureFiltering filtering) {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    GLBuffer *newAttach{nullptr};
    if(bufType == BufferType::Texture2D) {
        TextureParameters texParams;
        texParams.minFilter = filtering; // TODO: user should set filtration
        texParams.magFilter = filtering;
        texParams.desiredFormat = bufFormat;
        newAttach = new Texture2D(width, height, std::move(attachmentName), texParams);
        glFramebufferTexture2D(GL_FRAMEBUFFER, target,GL_TEXTURE_2D,
                               newAttach->GetHandle(), 0);
    }
    else if(bufType == BufferType::RenderBuffer) {
        newAttach = new RenderBuffer(width, height, std::move(attachmentName), bufFormat);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, target,
                                  GL_RENDERBUFFER, newAttach->GetHandle());
    }
    else {
        Debug::LogError("Framebuffer", "Unknown attachment buffer", attachmentName);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return newAttach;
}

void FrameBuffer::DisposeDepth() {
    if(depthAttachment.buffer
    && !depthAttachment.externalResource) {
        depthAttachment.buffer->Dispose();
        delete depthAttachment.buffer;
        depthAttachment.buffer = nullptr;
    }
}

void FrameBuffer::DisposeColor() {
    for(const auto &attach : colorAttachments) {
        if(!attach.externalResource) {
            attach.buffer->Dispose();
            delete attach.buffer;
        }
    }
    colorAttachments.clear();
}

void FrameBuffer::CreateColorAttachment(std::string attachmentName, BufferType bufType, BufferFormat bufFormat) {
    GLenum attachmentIndex = colorAttachments.size();
    GLenum attachmentTarget = GL_COLOR_ATTACHMENT0 + attachmentIndex;
    auto newAttach = CreateAttachment(attachmentTarget, std::move(attachmentName),
                                      bufType, bufFormat, TextureFiltering::Linear);
    if(newAttach) {
        colorAttachments.emplace_back(newAttach, false, attachmentIndex);
    }
}

void FrameBuffer::CreateDepthAttachment(std::string attachmentName, BufferType bufType, BufferFormat bufFormat) {
    DisposeDepth();
    GLenum attachmentTarget;
    switch (bufFormat) {
        case BufferFormat::Depth:
            attachmentTarget = GL_DEPTH_ATTACHMENT;
            break;
        case BufferFormat::DepthStencil:
            attachmentTarget = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        default:
            Debug::LogError("Framebuffer", "Depth attachment", "Unknown depth format", static_cast<int>(bufFormat));
            return;
    }

    auto newAttach = CreateAttachment(attachmentTarget, std::move(attachmentName),
                                      bufType, bufFormat,TextureFiltering::Nearest);
    if(newAttach) {
        depthAttachment.buffer = newAttach;
    }
}

void FrameBuffer::AddColorAttachment(GLBuffer *attachment, BufferType bufType) {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    GLenum attachmentTarget = GL_COLOR_ATTACHMENT0 + colorAttachments.size();
    switch (bufType) {
        case BufferType::Texture2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTarget, GL_TEXTURE_2D,
                                   attachment->GetHandle(), 0);
            break;
        case BufferType::RenderBuffer:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentTarget,
                                      GL_RENDERBUFFER, attachment->GetHandle());
            break;
        default:
            Debug::LogError("Framebuffer", "Color attachment", "Unknown format", attachment->GetName());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    colorAttachments.emplace_back(attachment, true);
}

GLuint FrameBuffer::GetColorAttachment(const std::string &attachName) const {
    auto targetAttachment = std::find_if(colorAttachments.begin(), colorAttachments.end(),
                                         [&attachName](const Attachment &attach){ return attach.buffer->GetName() == attachName; });
    return targetAttachment == colorAttachments.end() ? GL_NONE : targetAttachment->buffer->GetHandle();
}

GLuint FrameBuffer::GetDepthAttachment() const {
    return depthAttachment.buffer ? depthAttachment.buffer->GetHandle() : GL_NONE;
}

const std::vector<Attachment> &FrameBuffer::GetAllAttachments() const {
    return colorAttachments;
}

bool FrameBuffer::IsValid() const {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    bool isBufValid = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return isBufValid;
}

void FrameBuffer::Dispose() {
    DisposeColor();
    DisposeDepth();
    glDeleteFramebuffers(1, &handle);
}