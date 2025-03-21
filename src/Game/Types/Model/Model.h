#pragma once
#include <string>
#include "MeshNode.h"
#include "../../../Utils/Utils.h"
#include "../../../Dispose/IDisposable.h"

struct Model : public IDisposable {
    std::string name;
    MeshNode *root;

    explicit Model(std::string name, MeshNode *root = nullptr);
    void Dispose() override;
private:
    void DisposeNode(MeshNode *node);
};