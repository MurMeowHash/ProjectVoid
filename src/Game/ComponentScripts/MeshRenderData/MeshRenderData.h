#pragma once
#include "../ObjectComponent.h"
#include "../ComponentMacros.h"
#include <vector>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include "../../../Utils/Utils.h"
#include "../../../Renderer/RenderTypes.h"
#include "../../Types/BoundingBox/AABB.h"

class GameObject;

struct MeshRenderData : public ObjectComponent {
DEFINE_BASE(void)
    std::vector<uint> meshes;
    std::vector<RenderItem3D> renderItems;
    std::string modelName; // Назва моделі для відображення в UI

    explicit MeshRenderData(const std::vector<uint> &meshes = DEFAULT_MESHES);
    NODISCARD AABB GetCommonAABB() const;

    void Update() override;
    void RenderUI(GameObject* obj) override;
    GET_COMPONENT_TYPE_NAME(MeshRenderData)
    
    NODISCARD nlohmann::json SerializeToJson() const override;
    static MeshRenderData* CreateFromJson(GameObject* owner, const nlohmann::json& params);
private:
    static constexpr std::vector<uint> DEFAULT_MESHES = std::vector<uint>();
    void ConstructRenderItems();
};