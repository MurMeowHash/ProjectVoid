#include "GameObject.h"

GameObject::GameObject(const GameObjectParameters &params)
: name(params.name), transform{params.transform}, meshes(params.meshes), parent{params.parent} {

}

Transform GameObject::GetTransform() const {
    return transform;
}

std::string GameObject::GetName() const {
    return name;
}

void GameObject::SetTransform(const Transform &targetTransform) {
    transform = targetTransform;
}

void GameObject::SetPosition(const glm::vec3 &pos) {
    transform.position = pos;
}

void GameObject::SetName(std::string targetName) {
    name = std::move(targetName);
}

void GameObject::SetRotation(const glm::vec3 &rot) {
    transform.rotation = rot;
}

void GameObject::SetScale(const glm::vec3 &scl) {
    transform.scale = scl;
}
