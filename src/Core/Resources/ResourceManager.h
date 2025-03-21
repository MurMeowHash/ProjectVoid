#pragma once
#include "../../Game/Types/Texture/Texture2D.h"
#include <string>
#include "../../Utils/Utils.h"
#include "../../Game/Types/Model/Model.h"
#include "../../Game/Types/Model/Mesh.h"
#include "../../Game/Types/Material.h"

namespace ResourceManager {
    constexpr ModelLoadParameters DEFAULT_MODEL_LOAD_PARAMS = ModelLoadParameters();
    constexpr int ABSENT_RESOURCE = -1;

    void LoadModel(const char *path, const ModelLoadParameters &params = DEFAULT_MODEL_LOAD_PARAMS);
    int LoadTexture(const char *path, TextureParameters &params);
    void CreateTexture(const std::string &name, GLsizei width, GLsizei height,
                       const TextureParameters &params = TextureParameters(), unsigned char *data = nullptr);
    void CreateMaterial(const std::string &name, int albedoMap = ABSENT_RESOURCE, int normalMap =
            ABSENT_RESOURCE, int specularMap = ABSENT_RESOURCE, float shininess = Material::DEFAULT_SHININESS);
    Texture2D* GetTextureByIndex(int index);
    Model* GetModelByIndex(int index);
    Mesh* GetMeshByIndex(int index);
    Material *GetMaterialByIndex(int index);
    NODISCARD int GetTextureIndexByName(const std::string &name);
    NODISCARD int GetModelIndexByName(const std::string &name);
    NODISCARD int GetMeshIndexByName(const std::string &name);
    NODISCARD int GetMaterialIndexByName(const std::string &name);

    void LoadAssets();
    void Dispose();
}