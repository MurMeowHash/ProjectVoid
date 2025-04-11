#pragma once
#include <string>
#include "MeshNode.h"
#include "../../../Utils/Utils.h"
#include "../../../Dispose/IDisposable.h"

//TODO: set material for entire model

class Model : public IDisposable {
private:
    std::string name;
    MeshNode *root;

    void DisposeNode(MeshNode *node);
public:
    explicit Model(std::string name, MeshNode *root = nullptr);
    void SetRootNode(MeshNode *node);
    NODISCARD std::string GetName() const;
    NODISCARD MeshNode *GetRoot() const;
    void Dispose() override;
};