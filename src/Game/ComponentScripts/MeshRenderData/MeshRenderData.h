#pragma once
#include "../ObjectComponent.h"
#include <vector>
#include <string>
#include "../../../Utils/Utils.h"
#include "../../../Renderer/RenderTypes.h"
#include "../../Types/BoundingBox/AABB.h"

class GameObject;

struct MeshRenderData : public ObjectComponent {
DEFINE_BASE(void)
    std::vector<uint> meshes;
    std::vector<RenderItem3D> renderItems;
    std::string modelName;

    explicit MeshRenderData(const std::vector<uint> &meshes = DEFAULT_MESHES);
    NODISCARD AABB GetCommonAABB() const;

    void Update() override;
private:
    static constexpr std::vector<uint> DEFAULT_MESHES = std::vector<uint>();
    void ConstructRenderItems();
};