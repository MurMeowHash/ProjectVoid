#include "Model.h"

Model::Model(std::string name, MeshNode *root)
: name(std::move(name)), root{root} {

}

void Model::SetRootNode(MeshNode *node) {
    Dispose();
    root = node;
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