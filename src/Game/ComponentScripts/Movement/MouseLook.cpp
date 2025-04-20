#include "MouseLook.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"

MouseLook::MouseLook()
: ObjectComponent(0) {

}

void MouseLook::Update() {
    auto transform = GetGameObject()->GetComponent<Transform>();

    transform->rotation.x += Input::GetMouseOffsetY() * sensitivityX;
    transform->rotation.y -= Input::GetMouseOffsetX() * sensitivityY;

    transform->rotation.x = glm::clamp(transform->rotation.x, minVerticalAngle, maxVerticalAngle);
}