#include "MeshRenderData.h"
#include "../../../Core/Resources/ResourceManager.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../Scene/Scene.h"

MeshRenderData::MeshRenderData(const std::vector<uint> &meshes)
: ObjectComponent(2), meshes(meshes) {

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