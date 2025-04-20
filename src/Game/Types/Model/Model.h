#pragma once
#include <string>
#include "MeshNode.h"
#include "../../../Utils/Utils.h"
#include "../../../Dispose/IDisposable.h"

class Model : public IDisposable {
private:
    std::string name;
    MeshNode *root;

    void DisposeNode(MeshNode *node);

    static void SetMaterialUtil(MeshNode *node, int materialIndex);
public:
    explicit Model(std::string name, MeshNode *root = nullptr);
    void SetRootNode(MeshNode *node);
    void SetMaterial(int materialIndex);
    NODISCARD std::string GetName() const;
    NODISCARD MeshNode *GetRoot() const;
    void Dispose() override;
};