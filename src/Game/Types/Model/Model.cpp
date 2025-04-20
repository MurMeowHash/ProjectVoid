#include "Model.h"
#include "../../../Core/Resources/ResourceManager.h"

Model::Model(std::string name, MeshNode *root)
: name(std::move(name)), root{root} {

}

void Model::SetRootNode(MeshNode *node) {
    Dispose();
    root = node;
}

void Model::SetMaterialUtil(MeshNode *node, int materialIndex) {
    for(auto meshIndex : node->meshes) {
        ResourceManager::GetMeshByIndex(static_cast<int>(meshIndex))->SetMaterial(materialIndex);
    }

    for(auto child : node->children) {
        SetMaterialUtil(child, materialIndex);
    }
}

void Model::SetMaterial(int materialIndex) {
    SetMaterialUtil(root, materialIndex);
}

std::string Model::GetName() const {
    return name;
}

MeshNode *Model::GetRoot() const {
    return root;
}

void Model::Dispose() {
    DisposeNode(root);
    delete root;
}

void Model::DisposeNode(MeshNode *node) {
    if(node == nullptr) return;

    for(auto child : node->children) {
        DisposeNode(child);
        delete child;
    }
}