#include "Model.h"

Model::Model(std::string name, MeshNode *root)
: name(std::move(name)), root{root} {

}

void Model::Dispose() {
    DisposeNode(root);
    delete root;
}

void Model::DisposeNode(MeshNode *node) {
    for(auto child : node->children) {
        DisposeNode(child);
        delete child;
    }
}
