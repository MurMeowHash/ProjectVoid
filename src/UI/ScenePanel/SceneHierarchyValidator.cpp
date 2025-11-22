#include "SceneHierarchyValidator.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Utils/Utils.h"

bool SceneHierarchyValidator::CheckParentPath(const std::string& objName, const std::string& targetParent) {
    if(objName == targetParent) return true;
    
    auto* obj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(objName));
    if(!obj) return false;
    
    std::string parentName = obj->GetParentName();
    if(parentName.empty() || parentName == UNDEFINED_NAME) {
        return false;
    }
    
    return CheckParentPath(parentName, targetParent);
}

bool SceneHierarchyValidator::IsDescendant(const std::string& potentialParent, const std::string& child) {
    return CheckParentPath(child, potentialParent);
}

bool SceneHierarchyValidator::IsAncestorOf(const std::string& ancestor, const std::string& descendant) {
    if(descendant == ancestor) return true;
    
    auto* obj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(descendant));
    if(!obj) return false;
    
    std::string parentName = obj->GetParentName();
    if(parentName.empty() || parentName == UNDEFINED_NAME) {
        return false;
    }
    
    return IsAncestorOf(ancestor, parentName);
}

bool SceneHierarchyValidator::CanSetParent(const std::string& childName, const std::string& potentialParentName) {
    if(childName == potentialParentName) return false;
    
    auto* childObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(childName));
    if(!childObj) return false;
    
    std::string currentParent = childObj->GetParentName();
    if(currentParent.empty() || currentParent == UNDEFINED_NAME) {
        return !IsDescendant(childName, potentialParentName);
    }
    
    if(IsAncestorOf(potentialParentName, currentParent)) {
        return true;
    }
    
    return !IsDescendant(childName, potentialParentName);
}