#include "MeshRenderData.h"
#include "../../../Core/Resources/ResourceManager.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../Scene/Scene.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include "../../../Utils/Utils.h"
#include <imgui/imgui.h>
#include <algorithm>
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

void MeshRenderData::RenderUI(GameObject* obj) {
    std::string currentModelName = modelName;
    if(currentModelName.empty() && !meshes.empty()) {
        currentModelName = ResourceManager::GetModelNameByMeshes(meshes);
    }

    ImGui::Text("Select Model:");
    ImGui::SetNextItemWidth(-1);

    auto modelNames = ResourceManager::GetAllModelNames();
    if(modelNames.empty()) {
        ImGui::Text("No models loaded");
        return;
    }

    std::vector<const char*> modelNamesCStr;
    modelNamesCStr.reserve(modelNames.size() + 1);
    modelNamesCStr.push_back("None");
    for(const auto& name : modelNames) {
        modelNamesCStr.push_back(name.c_str());
    }

    int currentIndex = 0;
    if(!currentModelName.empty()) {
        auto it = std::find(modelNames.begin(), modelNames.end(), currentModelName);
        if(it != modelNames.end()) {
            currentIndex = static_cast<int>(std::distance(modelNames.begin(), it) + 1);
        }
    }

    if(ImGui::Combo("##ModelSelect", &currentIndex, modelNamesCStr.data(),
        static_cast<int>(modelNamesCStr.size()))) {
        if(currentIndex == 0) {
            meshes.clear();
            modelName.clear();
        } else {
            std::string selectedModelName = modelNames[currentIndex - 1];
            int modelIndex = ResourceManager::GetModelIndexByName(selectedModelName);
            if(modelIndex != ABSENT_RESOURCE) {
                auto* model = ResourceManager::GetModelByIndex(modelIndex);
                if(model) {
                    meshes.clear();
                    Scene::AddModelMeshesToGameObject(obj, model);
                    modelName = selectedModelName;
                }
            }
        }
    }
    
    ImGui::Spacing();
}

REGISTER_COMPONENT_FROM_JSON_WITH_UI(MeshRenderData, "Mesh Render Data")