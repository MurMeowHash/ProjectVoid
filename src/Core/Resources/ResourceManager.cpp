#include "ResourceManager.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../Debug/Debug.h"
#include <vector>
#include <unordered_map>
#include <stb/stb_image.h>

namespace ResourceManager {
    constexpr std::string RESOURCES_DIRECTORY = "../resources/";

    std::vector<Model> models;
    std::vector<Mesh> meshes;
    std::vector<Texture2D> textures;
    std::vector<Material> materials;
    std::unordered_map<std::string, uint> meshIndexMap;
    std::unordered_map<std::string, uint> modelIndexMap;
    std::unordered_map<std::string, uint> textureIndexMap;
    std::unordered_map<std::string, uint> materialIndexMap;

    NODISCARD std::string ConstructFullPath(const char *path);

    template<typename T, typename ... Args>
    uint AddResource(std::vector<T> &resHolder, std::unordered_map<std::string, uint> &resIndexMap, const Args& ... args);
    MeshNode *ProcessNode(aiNode *node, const aiScene *scene, const std::string &modelTexturesDirectory,
                          const std::string &modelFormat, const ModelLoadParameters &params);
    uint CreateMesh(const aiMesh *mesh, uint material);
    void CreateModel(const aiScene *scene, const char *path, const ModelLoadParameters &params);
    NODISCARD int GetIndexByName(const std::string &name, const std::unordered_map<std::string, uint> &targetMap);
    int LoadTextureFromAssimp(aiMaterial *mat, aiTextureType texType, const std::string &modelTexturesDirectory);
    uint CreateMaterialFromAssimp(aiMaterial *mat, const std::string &modelTexturesDirectory, const std::string &modelFormat);
    void CreateScreenPlane();

    void LoadModel(const char *path, const ModelLoadParameters &params) {
        std::string fullPath = ConstructFullPath(path);
        Assimp::Importer modelImporter;
        auto scene = modelImporter.ReadFile(fullPath,
                                            aiProcess_FlipUVs| aiProcess_Triangulate | aiProcess_CalcTangentSpace);
        if(!scene) {
            Debug::LogError("Assimp", path, modelImporter.GetErrorString());
            return;
        }

        if(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode) {
            Debug::LogError("Assimp", path, modelImporter.GetErrorString());
            return;
        }

        CreateModel(scene, path, params);
    }

    int LoadTexture(const char *path, TextureParameters &params) {
        std::string textureName = Utils::GetNameFromPath(path);

        if(int texIndex = GetTextureIndexByName(textureName); texIndex != ABSENT_RESOURCE) {
            Debug::LogError("Texture", path, "Already exists");
            return texIndex;
        }

        int imWidth, imHeight, nrChannels;
        auto imageData = stbi_load(ConstructFullPath(path).c_str(), &imWidth, &imHeight,
                                   &nrChannels, 0);
        if(!imageData) {
            Debug::LogError("Texture", "Failed to load image", path);
            return ABSENT_RESOURCE;
        }
        GLint internalFormat;
        switch (nrChannels) {
            case 1:
                internalFormat = GL_RED;
                break;
            case 3:
                internalFormat = GL_RGB;
                break;
            case 4:
                internalFormat = GL_RGBA;
                break;
            default:
                Debug::LogError("Texture", "Unknown number of channels", path, nrChannels);
                return ABSENT_RESOURCE;
        }

        if(params.desiredFormat == BufferFormat::SRGB
        || params.desiredFormat == BufferFormat::SRGBA) {
            if(internalFormat == GL_RGB) {
                internalFormat = GL_SRGB;
            } else if(internalFormat == GL_RGBA) {
                internalFormat = GL_SRGB_ALPHA;
            }
        }

        params.desiredFormat = static_cast<BufferFormat>(internalFormat);
        CreateTexture(textureName, imWidth, imHeight, params, imageData);
        stbi_image_free(imageData);
        return static_cast<int>(textures.size() - 1);
    }

    void CreateTexture(const std::string &name, GLsizei width, GLsizei height,
                       const TextureParameters &params, unsigned char *data) {
        if(GetTextureIndexByName(name) != ABSENT_RESOURCE) {
            Debug::LogError("Texture", name, "Already exists");
            return;
        }

        AddResource(textures, textureIndexMap, width, height, name, params, data);
    }

    void CreateMaterial(const std::string &name, int albedoMap, int normalMap, int specularMap, float shininess) {
        if(GetMaterialIndexByName(name) != ABSENT_RESOURCE) {
            Debug::LogError("Material", name, "Already exists");
            return;
        }

        AddResource(materials, materialIndexMap, name, albedoMap, normalMap, specularMap, shininess);
    }

    void CreateManualMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint> &indices,
                          bool hasNormals, bool hasTangents) {
        if(GetMeshIndexByName(name) != ABSENT_RESOURCE) {
            Debug::LogError("Mesh", name, "Already exists");
            return;
        }

        AddResource(meshes, meshIndexMap, name, vertices, indices, hasNormals, hasTangents);
    }

    Texture2D* GetTextureByIndex(int index) {
        return (index < 0 || index >= textures.size()) ? nullptr : &textures[index];
    }

    Model* GetModelByIndex(int index) {
        return (index < 0 || index >= models.size()) ? nullptr : &models[index];
    }

    Mesh* GetMeshByIndex(int index) {
        return (index < 0 || index >= meshes.size()) ? nullptr : &meshes[index];
    }

    Material *GetMaterialByIndex(int index) {
        return (index < 0 || index >= materials.size()) ? nullptr : &materials[index];
    }

    int GetTextureIndexByName(const std::string &name) {
        return GetIndexByName(name, textureIndexMap);
    }

    int GetModelIndexByName(const std::string &name) {
        return GetIndexByName(name, modelIndexMap);
    }

    int GetMeshIndexByName(const std::string &name) {
        return GetIndexByName(name, meshIndexMap);
    }

    int GetMaterialIndexByName(const std::string &name) {
        return GetIndexByName(name, materialIndexMap);
    }

    std::string ConstructFullPath(const char *path) {
        return RESOURCES_DIRECTORY + path;
    }

    template<typename T, typename ... Args>
    uint AddResource(std::vector<T> &resHolder, std::unordered_map<std::string, uint> &resIndexMap, const Args& ... args) {
        auto res = resHolder.emplace_back(args...);
        uint lastResourceIndex = resHolder.size() - 1;
        resIndexMap[res.GetName()] = lastResourceIndex;
        return lastResourceIndex;
    }

    MeshNode *ProcessNode(aiNode *node, const aiScene *scene, const std::string &modelTexturesDirectory,
                          const std::string &modelFormat, const ModelLoadParameters &params) {
        auto meshNode = new MeshNode;
        meshNode->name = node->mName.C_Str();
        aiVector3D position, rotation, scale;
        node->mTransformation.Decompose(scale, rotation, position);
        float targetScaleFactor = node == scene->mRootNode ? 1.0f : params.scaleFactor;
        meshNode->transform = Transform{Utils::ToGLMVector(position),
                              glm::degrees(Utils::ToGLMVector(rotation)),
                              Utils::ToGLMVector(scale) * targetScaleFactor};
        meshNode->meshes.reserve(node->mNumMeshes);
        for(uint i = 0; i < node->mNumMeshes; i++) {
            auto currentMesh = scene->mMeshes[node->mMeshes[i]];
            auto material = CreateMaterialFromAssimp(scene->mMaterials[currentMesh->mMaterialIndex],
                                                     modelTexturesDirectory, modelFormat);
            meshNode->meshes.emplace_back(CreateMesh(currentMesh, material));
        }

        meshNode->children.reserve(node->mNumChildren);
        for(uint i = 0; i < node->mNumChildren; i++) {
            meshNode->children.push_back(ProcessNode(node->mChildren[i], scene, modelTexturesDirectory, modelFormat, params));
        }

        return meshNode;
    }

    uint CreateMesh(const aiMesh *mesh, uint material) {
        if(int meshIndex = GetMeshIndexByName(mesh->mName.C_Str()); meshIndex != ABSENT_RESOURCE) {
            Debug::LogError("Mesh", mesh->mName.C_Str(), "Already exists");
            return meshIndex;
        }

        return AddResource(meshes, meshIndexMap, *mesh, material);
    }

    void CreateModel(const aiScene *scene, const char *path, const ModelLoadParameters &params) {
        std::string modelPath(path);
        std::string modelName = Utils::GetNameFromPath(modelPath);
        std::string modelTexturesDirectory = modelPath.substr(0, Utils::FindLastSlash(modelPath)) + '/' + params.textureDirectory;
        std::string modelFormat = Utils::GetExtensionFromPath(path);

        if(GetModelIndexByName(modelName) != ABSENT_RESOURCE) {
            Debug::LogError("Model", modelName, "Already exists");
            return;
        }

        auto modelIndex = static_cast<int>(AddResource(models, modelIndexMap, modelName));
        auto model = GetModelByIndex(modelIndex);
        scene->mRootNode->mName = modelName;
        model->SetRootNode(ProcessNode(scene->mRootNode, scene, modelTexturesDirectory, modelFormat, params));
    }

    int GetIndexByName(const std::string &name, const std::unordered_map<std::string, uint> &targetMap) {
        auto iterator = targetMap.find(name);
        if(iterator != targetMap.end()) {
            return static_cast<int>(iterator->second);
        }

        return ABSENT_RESOURCE;
    }

    int LoadTextureFromAssimp(aiMaterial *mat, aiTextureType texType, const std::string &modelTexturesDirectory) {
        uint textureCount = mat->GetTextureCount(texType);
        if(textureCount == 0) {
            return ABSENT_RESOURCE;
        }

        if(textureCount > 1) {
            Debug::LogWarning("Material", mat->GetName().C_Str(), "More than 1 diffuse texture encountered which will be lost");
        }

        aiString rawTexturePath;
        mat->GetTexture(texType, 0, &rawTexturePath);
        std::string texturePath = modelTexturesDirectory + Utils::GetExtendedNameFromPath(rawTexturePath.C_Str());
        TextureParameters texParams;
        texParams.minFilter = TextureFiltering::LinearMipMap;
        texParams.magFilter = TextureFiltering::Linear;
        texParams.genMipMaps = true;
        return LoadTexture(texturePath.c_str(), texParams);
    }

    uint CreateMaterialFromAssimp(aiMaterial *mat, const std::string &modelTexturesDirectory, const std::string &modelFormat) {
        if(int matIndex = GetMaterialIndexByName(mat->GetName().C_Str()); matIndex != ABSENT_RESOURCE) {
            Debug::LogError("Material", mat->GetName().C_Str(), "Already exists");
            return matIndex;
        }

        int diffuseMap = LoadTextureFromAssimp(mat, aiTextureType::aiTextureType_DIFFUSE, modelTexturesDirectory);
        int normalMap = LoadTextureFromAssimp(mat, aiTextureType::aiTextureType_NORMALS, modelTexturesDirectory);
        if(normalMap == ABSENT_RESOURCE) {
            normalMap = LoadTextureFromAssimp(mat, aiTextureType::aiTextureType_HEIGHT, modelTexturesDirectory);
        }
        int specularMap = LoadTextureFromAssimp(mat, aiTextureType::aiTextureType_SPECULAR, modelTexturesDirectory);

        float shininess;
        if(mat->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) {
            shininess = Material::DEFAULT_SHININESS;
        }
        shininess = Material::NormalizeShininess(shininess, modelFormat);

        CreateMaterial(mat->GetName().C_Str(), diffuseMap, normalMap, specularMap, shininess);
        return materials.size() - 1;
    }

    void CreateScreenPlane() {
        std::vector<Vertex> vertices(4);
        //upper right corner
        vertices[0].position = glm::vec3(1.0f, 1.0f, 0.0f);
        vertices[0].texCoords = glm::vec2(1.0f, 1.0f);
        //upper left corner
        vertices[1].position = glm::vec3(-1.0f, 1.0f, 0.0f);
        vertices[1].texCoords = glm::vec2(0.0f, 1.0f);
        //lower left corner
        vertices[2].position = glm::vec3(-1.0f, -1.0f, 0.0f);
        vertices[2].texCoords = glm::vec2(0.0f, 0.0f);
        //lower right corner
        vertices[3].position = glm::vec3(1.0f, -1.0f, 0.0f);
        vertices[3].texCoords = glm::vec2(1.0f, 0.0f);

        std::vector<uint> indices = { 0, 1, 2, 0, 2, 3 };

        CreateManualMesh("ScreenPlane", vertices, indices);
    }

    void LoadAssets() {
        CreateScreenPlane();
        ModelLoadParameters loadParam {0.01f};
        LoadModel("Models/Soldier/soldier.fbx", loadParam);
    }

    void Dispose() {
        for(auto &model : models) {
            model.Dispose();
        }

        for(auto &mesh : meshes) {
            mesh.Dispose();
        }

        for(auto &texture : textures) {
            texture.Dispose();
        }
    }
}