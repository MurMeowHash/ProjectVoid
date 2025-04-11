#include "Renderer.h"
#include "RenderData.h"
#include "../Shader/Shader.h"
#include "Buffers/UBO.h"
#include "../Core/Resources/ResourceManager.h"
#include "../Debug/Debug.h"
#include "../Game/Scene/Scene.h"
#include "../Error/Error.h"
#include "../Game/PostProcessing/PostProcessing.h"

namespace Renderer {
    const std::string SHADERS_DIRECTORY = "../src/Shader/GLSLShaders/";

    int prevAlbedoMap = ABSENT_RESOURCE;
    int prevNormalMap = ABSENT_RESOURCE;
    int prevSpecularMap = ABSENT_RESOURCE;

    std::vector<FrameBuffer> userFrameBuffers;
    std::unordered_map<uint, uint> userFrameBuffersIndexMaps;
    FrameBuffer *outputFrameBuffer = nullptr;

    struct Shaders {
        Shader geometryShader;
        Shader indirectLightingShader;
        Shader postProcessingShader;
    } shaders;

    struct UBOs {
        UBO transformMatricesBuffer;
        UBO lightsBuffer;
    } ubos;

    struct FrameBuffers {
        FrameBuffer gBuffer;
        FrameBuffer screenBuffer;
    } engineFrameBuffers;

    NODISCARD std::string ConstructFullShaderPath(const char *shaderPath);
    void LoadShaders();
    void InitializeShaders();
    void InitializeBuffers();
    void TrackFrameBufferValid(const FrameBuffer &buf);
    void BindFrameBuffer(const FrameBuffer *buf);
    RenderData ConstructRenderData();
    void SetMap(int mapIndex, GLenum unit, int &prevMapIndex);
    void ClearFramebuffer(const glm::vec3 &color = glm::vec3(0.0f));
    void GeometryPass(const RenderData &renderData);
    void IndirectLightingPass(const RenderData &renderData, const GPUCamera &activeCam);
    void PostProcessingPass(const RenderData &renderData);
    void DrawScreenPlane();

    std::string ConstructFullShaderPath(const char *shaderPath) {
        return SHADERS_DIRECTORY + shaderPath;
    }

    void LoadShaders() {
        shaders.geometryShader.Load(ConstructFullShaderPath("IndirectShading/geometry.vert").c_str(),
                                    ConstructFullShaderPath("IndirectShading/geometry.frag").c_str());
        shaders.indirectLightingShader.Load(ConstructFullShaderPath("screenProcess.vert").c_str(),
                                            ConstructFullShaderPath("IndirectShading/lighting.frag").c_str());
        shaders.postProcessingShader.Load(ConstructFullShaderPath("screenProcess.vert").c_str(),
                                          ConstructFullShaderPath("PostProcessing/postProcess.frag").c_str());
    }

    void InitializeShaders() {
        //geometry
        shaders.geometryShader.Bind();
        shaders.geometryShader.SetInt("albedoMap", 0);
        shaders.geometryShader.SetInt("normalMap", 1);
        shaders.geometryShader.SetInt("specularMap", 2);

        //indirect lighting
        shaders.indirectLightingShader.Bind();
        shaders.indirectLightingShader.SetInt("gPosition", 0);
        shaders.indirectLightingShader.SetInt("gNormalShininess", 1);
        shaders.indirectLightingShader.SetInt("gAlbedoSpec", 2);

        //post processing
        shaders.postProcessingShader.Bind();
        shaders.postProcessingShader.SetInt("screenTexture", 0);

        Shader::UnBind();
    }

    void InitializeBuffers() {
        //UBOs
        ubos.transformMatricesBuffer.Create(0, 128);
        ubos.lightsBuffer.Create(1, 16000);

        //FBOs
        GLsizei screenWidth = Core::GetScreenWidth();
        GLsizei screenHeight = Core::GetScreenHeight();
        //geometry framebuffer
        engineFrameBuffers.gBuffer.Create(screenWidth, screenHeight, "gBuffer");
        engineFrameBuffers.gBuffer.CreateColorAttachment("Position", BufferType::Texture2D,
                                                         BufferFormat::RGBA16F);
        engineFrameBuffers.gBuffer.CreateColorAttachment("NormalShininess", BufferType::Texture2D,
                                                         BufferFormat::RGBA16F);
        engineFrameBuffers.gBuffer.CreateColorAttachment("AlbedoSpec", BufferType::Texture2D,
                                                         BufferFormat::RGBA);
        engineFrameBuffers.gBuffer.CreateDepthAttachment("gBufferDepth", BufferType::RenderBuffer,
                                                         BufferFormat::Depth);
        TrackFrameBufferValid(engineFrameBuffers.gBuffer);

        //screen framebuffer
        engineFrameBuffers.screenBuffer.Create(screenWidth, screenHeight, "ScreenBuffer");
        engineFrameBuffers.screenBuffer.CreateColorAttachment("ScreenTexture", BufferType::Texture2D,
                                                              BufferFormat::RGBA16F);
        engineFrameBuffers.screenBuffer.CreateDepthAttachment("ScreenDepth", BufferType::RenderBuffer,
                                                              BufferFormat::Depth);
        TrackFrameBufferValid(engineFrameBuffers.screenBuffer);
    }

    void TrackFrameBufferValid(const FrameBuffer &buf) {
        if(!buf.IsValid()) {
            Error::FallWithFatalError("Framebuffer", buf.GetName(), "Incomplete");
        }
    }

    void BindFrameBuffer(const FrameBuffer *buf) {
        if(!buf) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Core::GetScreenWidth(), Core::GetScreenHeight());
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, buf->GetHandle());
        glViewport(0, 0, buf->GetWidth(), buf->GetHeight());
        const auto &bufferAttachments = buf->GetAllAttachments();
        std::vector<GLenum> renderTargets;
        renderTargets.reserve(bufferAttachments.size());
        for(const auto &attach : bufferAttachments) {
            renderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attach.index);
        }

        glDrawBuffers(static_cast<GLsizei>(renderTargets.size()), renderTargets.data());
    }

    RenderData ConstructRenderData() {
        RenderData renderData;
        renderData.geometryRenderItems = Scene::GetGeometryRenderItems();
        renderData.gpuCameras = Scene::GetGPUCameras();
        renderData.gpuLights = Scene::GetGPULights();
        renderData.environmentAmbient = Scene::GetEnvironmentAmbient();
        renderData.ppInfo.gamma = PostProcessing::GetGamma();
        renderData.ppInfo.exposure = PostProcessing::GetExposure();

        return renderData;
    }

    void SetMap(int mapIndex, GLenum unit, int &prevMapIndex) {
        if(mapIndex == prevMapIndex
           || mapIndex == ABSENT_RESOURCE) {
            return;
        }

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTextureByIndex(mapIndex)->GetHandle());
        prevMapIndex = mapIndex;
    }

    void ClearFramebuffer(const glm::vec3 &color) {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GeometryPass(const RenderData &renderData) {
        BindFrameBuffer(&engineFrameBuffers.gBuffer);
        ClearFramebuffer();
        shaders.geometryShader.Bind();

        for(const auto &renderItem : renderData.geometryRenderItems) {
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(renderItem.modelMatrix)));
            shaders.geometryShader.SetMat4("model", renderItem.modelMatrix);
            shaders.geometryShader.SetMat3("normal", normalMatrix);
            shaders.geometryShader.SetInt("hasAlbedoMap", renderItem.albedoMap != ABSENT_RESOURCE);
            shaders.geometryShader.SetInt("hasNormalMap", renderItem.normalMap != ABSENT_RESOURCE);
            shaders.geometryShader.SetInt("hasSpecularMap", renderItem.specularMap != ABSENT_RESOURCE);
            shaders.geometryShader.SetFloat("shininess", renderItem.shininess);

            SetMap(renderItem.albedoMap, 0, prevAlbedoMap);
            SetMap(renderItem.normalMap, 1, prevNormalMap);
            SetMap(renderItem.specularMap, 2, prevSpecularMap);

            glBindVertexArray(renderItem.vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderItem.countIndices), GL_UNSIGNED_INT, nullptr);
        }
        Shader::UnBind();
        BindFrameBuffer(outputFrameBuffer);
    }

    void IndirectLightingPass(const RenderData &renderData, const GPUCamera &activeCam) {
        BindFrameBuffer(&engineFrameBuffers.screenBuffer);
        ClearFramebuffer();
        shaders.indirectLightingShader.Bind();
        shaders.indirectLightingShader.SetInt("countLights", static_cast<int>(renderData.gpuLights.size()));
        shaders.indirectLightingShader.SetVec3("environmentAmbient", renderData.environmentAmbient);
        shaders.indirectLightingShader.SetVec3("viewPos", activeCam.position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, engineFrameBuffers.gBuffer.GetColorAttachment("Position"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, engineFrameBuffers.gBuffer.GetColorAttachment("NormalShininess"));
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, engineFrameBuffers.gBuffer.GetColorAttachment("AlbedoSpec"));

        DrawScreenPlane();

        Shader::UnBind();
        BindFrameBuffer(outputFrameBuffer);
    }

    void PostProcessingPass(const RenderData &renderData) {
        ClearFramebuffer();
        shaders.postProcessingShader.Bind();
        shaders.postProcessingShader.SetFloat("exposure", renderData.ppInfo.exposure);
        shaders.postProcessingShader.SetFloat("gamma", renderData.ppInfo.gamma);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, engineFrameBuffers.screenBuffer.GetColorAttachment("ScreenTexture"));

        DrawScreenPlane();
        Shader::UnBind();
    }

    void Initialize() {
        glEnable(GL_DEPTH_TEST);

        InitializeBuffers();
        LoadShaders();
        InitializeShaders();
    }

    void RenderFrame() {
        auto renderData = ConstructRenderData();

        //TODO: dirty flags optimization
        ubos.lightsBuffer.SetData(0, static_cast<GLsizeiptr>(renderData.gpuLights.size() * sizeof(GPULight)),
                                  renderData.gpuLights.data());
        for(const auto &camera : renderData.gpuCameras) {
            outputFrameBuffer = GetFrameBufferByIndex(camera.renderTarget);
            ubos.transformMatricesBuffer.SetMat4(0, camera.projectionMatrix);
            ubos.transformMatricesBuffer.SetMat4(MAT4_SIZE, camera.viewMatrix);

            GeometryPass(renderData);
            IndirectLightingPass(renderData, camera);
            //TODO: blit depth buffer to main output
            PostProcessingPass(renderData);
        }
    }

    void Dispose() {
        //Shaders
        shaders.geometryShader.Dispose();
        shaders.indirectLightingShader.Dispose();
        shaders.postProcessingShader.Dispose();

        //UBOs
        ubos.transformMatricesBuffer.Dispose();
        ubos.lightsBuffer.Dispose();

        //FBOs
        engineFrameBuffers.gBuffer.Dispose();
        engineFrameBuffers.screenBuffer.Dispose();
        for(auto &buffer : userFrameBuffers) {
            buffer.Dispose();
        }
    }

    int GetUserFrameBufferByTexture(uint textureIndex) {
        auto bufferIterator = userFrameBuffersIndexMaps.find(textureIndex);
        if(bufferIterator != userFrameBuffersIndexMaps.end()) {
            return static_cast<int>(bufferIterator->second);
        }

        return DEFAULT_RENDER_TARGET;
    }

    FrameBuffer *GetFrameBufferByIndex(int bufIndex) {
        return (bufIndex < 0 || bufIndex >= userFrameBuffers.size()) ? nullptr : &userFrameBuffers[bufIndex];
    }

    int CreateUserFrameBuffer(uint textureIndex) {
        if(auto bufferIndex = GetUserFrameBufferByTexture(textureIndex); bufferIndex != DEFAULT_RENDER_TARGET) {
            return bufferIndex;
        }

        auto texture = ResourceManager::GetTextureByIndex(static_cast<int>(textureIndex));
        if(!texture) {
            Debug::LogError("Framebuffer", "Failed to create", "texture is out of range", texture);
            return DEFAULT_RENDER_TARGET;
        }

        auto &userFrameBuffer = userFrameBuffers.emplace_back(texture->GetWidth(), texture->GetHeight());
        auto currentBufferIndex = userFrameBuffers.size() - 1;
        userFrameBuffersIndexMaps[textureIndex] = currentBufferIndex;
        userFrameBuffer.AddColorAttachment(texture, BufferType::Texture2D);
        userFrameBuffer.CreateDepthAttachment("userDepth", BufferType::RenderBuffer, BufferFormat::Depth);
        if(!userFrameBuffer.IsValid()) {
            Debug::LogError("Framebuffer", userFrameBuffer.GetName(), "is not valid", texture->GetName());
        }
        return static_cast<int>(currentBufferIndex);
    }

    void DrawScreenPlane() {
        auto screenPlane = ResourceManager::GetMeshByIndex(ResourceManager::GetMeshIndexByName("ScreenPlane"));
        glBindVertexArray(screenPlane->GetHandle());
        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(screenPlane->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
        glEnable(GL_DEPTH_TEST);
    }
}