#pragma once
#include "../../Game/Types/GLBuffer/Texture/Texture2D.h"
#include <string>
#include "../../Utils/Utils.h"
#include "../../Game/Types/Model/Model.h"
#include "../../Game/Types/Model/Mesh.h"
#include "../../Game/Types/Material.h"

namespace ResourceManager {
    constexpr ModelLoadParameters DEFAULT_MODEL_LOAD_PARAMS = ModelLoadParameters();

    void LoadModel(const char *path, const ModelLoadParameters &params = DEFAULT_MODEL_LOAD_PARAMS);
    int LoadTexture(const char *path, TextureParameters &params);
    void CreateTexture(const std::string &name, GLsizei width, GLsizei height,
                       const TextureParameters &params = TextureParameters(), unsigned char *data = nullptr);
    void CreateMaterial(const std::string &name, int albedoMap = ABSENT_RESOURCE, int normalMap =
            ABSENT_RESOURCE, int specularMap = ABSENT_RESOURCE, float shininess = Material::DEFAULT_SHININESS);
    void CreateManualMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint> &indices,
                          bool hasNormals = false, bool hasTangents = false);
    Texture2D* GetTextureByIndex(int index);
    Model* GetModelByIndex(int index);
    Mesh* GetMeshByIndex(int index);
    Material *GetMaterialByIndex(int index);
    NODISCARD int GetTextureIndexByName(const std::string &name);
    NODISCARD int GetModelIndexByName(const std::string &name);
    NODISCARD int GetMeshIndexByName(const std::string &name);
    NODISCARD int GetMaterialIndexByName(const std::string &name);
    NODISCARD std::string GetModelNameByMeshes(const std::vector<uint>& meshes);
    NODISCARD std::vector<std::string> GetAllModelNames();

    void LoadAssets();
    void Dispose();
}