#pragma once
#include <string>
#include <functional>

class SceneHierarchyValidator {
public:
    static bool IsDescendant(const std::string& potentialParent, const std::string& child);
    static bool CanSetParent(const std::string& childName, const std::string& potentialParentName);
    
private:
    static bool CheckParentPath(const std::string& objName, const std::string& targetParent);
    static bool IsAncestorOf(const std::string& ancestor, const std::string& descendant);
};