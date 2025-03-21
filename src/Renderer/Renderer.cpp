#include "Renderer.h"
#include "RenderData.h"
#include "../Shader/Shader.h"
#include "Buffers/UBO.h"
#include "../Core/Resources/ResourceManager.h"
#include "../Debug/Debug.h"

namespace Renderer {
    const std::string SHADERS_DIRECTORY = "../src/Shader/GLSLShaders/";

    struct Shaders {
        Shader geometryShader;
    } shaders;

    struct UBOs {
        UBO transformMatricesBuffer;
    } ubos;

    NODISCARD std::string ConstructFullShaderPath(const char *shaderPath);
    void LoadShaders();
    void InitializeShaders();
    void InitializeBuffers();
    void GeometryPass(const RenderData &renderData);

    std::string ConstructFullShaderPath(const char *shaderPath) {
        return SHADERS_DIRECTORY + shaderPath;
    }

    void LoadShaders() {
        shaders.geometryShader.Load(ConstructFullShaderPath("geometry.vert").c_str(),
                                    ConstructFullShaderPath("geometry.frag").c_str());
    }

    void InitializeShaders() {
        //geometry
        shaders.geometryShader.Bind();
        shaders.geometryShader.SetInt("albedoMap", 0);
        shaders.geometryShader.SetInt("normalMap", 1);
        shaders.geometryShader.SetInt("specularMap", 2);
        Shader::UnBind();
    }

    void InitializeBuffers() {
        ubos.transformMatricesBuffer.Create(0, 128);
    }

    void GeometryPass(const RenderData &renderData) {
        shaders.geometryShader.Bind();
        for(const auto &renderItem : renderData.geometryRenderItems) {
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(renderItem.modelMatrix)));
            shaders.geometryShader.SetMat4("model", renderItem.modelMatrix);
            shaders.geometryShader.SetMat3("normal", normalMatrix);
            //TODO: deal with ts shit in a nicer way + consider all maps
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTextureByIndex(renderItem.albedoMap)->GetHandle());
            glBindVertexArray(renderItem.vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderItem.countIndices), GL_UNSIGNED_INT, nullptr);
        }
        Shader::UnBind();
    }

    void Initialize() {
        glEnable(GL_DEPTH_TEST);

        InitializeBuffers();
        LoadShaders();
        InitializeShaders();
    }

    void RenderFrame(const RenderData &renderData) {
        ubos.transformMatricesBuffer.SetMat4(0, renderData.mainCameraData.projectionMatrix);
        ubos.transformMatricesBuffer.SetMat4(MAT4_SIZE, renderData.mainCameraData.viewMatrix);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GeometryPass(renderData);
    }

    void Dispose() {
        shaders.geometryShader.Dispose();
        ubos.transformMatricesBuffer.Dispose();
    }
}