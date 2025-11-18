#include "MeshRenderData.h"
#include "../../../Core/Resources/ResourceManager.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../Scene/Scene.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

MeshRenderData::MeshRenderData(const std::vector<uint> &meshes)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 3), meshes(meshes) {

}

void MeshRenderData::ConstructRenderItems() {
    glm::mat4 targetModelMatrix = glm::mat4(1.0f);

    auto owner = GetGameObject();
    if(owner) {
        targetModelMatrix = owner->GetComponent<Transform>()->worldModelMatrix;
    }

    renderItems.clear();

    for(auto meshIndex : meshes) {
        auto mesh = ResourceManager::GetMeshByIndex(static_cast<int>(meshIndex));
        if(mesh) {
            RenderItem3D item;
            item.vao = mesh->GetHandle();
            item.countIndices = mesh->GetIndicesCount();
            auto meshMaterial = ResourceManager::GetMaterialByIndex(mesh->GetMaterial());
            if(meshMaterial) {
                item.albedoMap = meshMaterial->GetAlbedoMap();
                item.normalMap = meshMaterial->GetNormalMap();
                item.specularMap = meshMaterial->GetSpecularMap();
                item.shininess = meshMaterial->GetShininess();
            }
            item.modelMatrix = targetModelMatrix;
            renderItems.emplace_back(item);
        }
    }
}

void MeshRenderData::Update() {
    ConstructRenderItems();
}

AABB MeshRenderData::GetCommonAABB() const {
    glm::vec3 minCommonVert(0.0f), maxCommonVert(0.0f);
    for(auto meshIndex : meshes) {
        const auto &meshAABB = ResourceManager::GetMeshByIndex(static_cast<int>(meshIndex))->GetAABB();
        minCommonVert = glm::min(minCommonVert, meshAABB.GetMinVertex());
        maxCommonVert = glm::max(maxCommonVert, meshAABB.GetMaxVertex());
    }

    return {minCommonVert, maxCommonVert};
}

nlohmann::json MeshRenderData::SerializeToJson() const {
    nlohmann::json params;
    // Серіалізуємо меші як масив індексів
    if(!meshes.empty()) {
        params["meshes"] = meshes;
    }
    // Серіалізуємо назву моделі, якщо вона є
    if(!modelName.empty()) {
        params["modelName"] = modelName;
    }
    return params;
}

MeshRenderData* MeshRenderData::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    std::vector<uint> meshes;
    if(params.contains("meshes") && params["meshes"].is_array()) {
        meshes = params["meshes"].get<std::vector<uint>>();
    }
    auto* meshRenderData = owner->AddComponent<MeshRenderData>(meshes);
    // Відновлюємо назву моделі, якщо вона є
    if(params.contains("modelName") && params["modelName"].is_string()) {
        meshRenderData->modelName = params["modelName"].get<std::string>();
    }
    return meshRenderData;
}

REGISTER_COMPONENT_FROM_JSON_WITH_UI(MeshRenderData, "Mesh Render Data")