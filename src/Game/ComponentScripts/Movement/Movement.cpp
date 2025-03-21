#include "Movement.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Engine/Time/Time.h"
#include "../../../Debug/Debug.h"

Movement::Movement()
: ObjectComponent(0) {
    //TODO: update priority of transform
}

void Movement::Update() {
    auto movementDirection = glm::vec3(0.0f);
    auto transform = GetGameObject()->GetComponent<Transform>();

    if(Input::GetKey(InputKey::KeyW)) {
        movementDirection += transform->ToForwardVector();
    }
    if(Input::GetKey(InputKey::KeyS)) {
        movementDirection -= transform->ToForwardVector();
    }
    if(Input::GetKey(InputKey::KeyD)) {
        movementDirection += transform->ToRightVector();
    }
    if(Input::GetKey(InputKey::KeyA)) {
        movementDirection -= transform->ToRightVector();
    }

    transform->position += movementDirection * speed * Time::GetDeltaTime();
}