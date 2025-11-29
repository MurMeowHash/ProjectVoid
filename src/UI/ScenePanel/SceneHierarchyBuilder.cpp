#include "SceneHierarchyBuilder.h"

#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Utils/Utils.h"
#include <functional>

namespace SceneHierarchyBuilder {
    std::vector<GameObjectNode> Build() {
        std::vector<GameObjectNode> rootNodes;
        std::vector<std::pair<GameObjectNode, std::string>> nodeParentPairs;

        for(int i = 0; i <= static_cast<int>(Scene::GetLastGameObjectIndex()); ++i) {
            auto* obj = Scene::GetGameObjectByIndex(i);
            if(!obj) continue;

            GameObjectNode node;
            node.name = obj->GetName();
            node.index = i;

            std::string parentName = obj->GetParentName();
            if(parentName.empty() || parentName == UNDEFINED_NAME) {
                rootNodes.push_back(node);
            } else {
                nodeParentPairs.push_back({node, parentName});
            }
        }

        for(const auto& [node, parentName] : nodeParentPairs) {
            std::function<bool(std::vector<GameObjectNode>&)> addChild = [&](std::vector<GameObjectNode>& nodes) -> bool {
                for(auto& n : nodes) {
                    if(n.name == parentName) {
                        n.children.push_back(node);
                        return true;
                    }
                    if(addChild(n.children)) {
                        return true;
                    }
                }
                return false;
            };

            if(!addChild(rootNodes)) {
                rootNodes.push_back(node);
            }
        }

        return rootNodes;
    }
}

