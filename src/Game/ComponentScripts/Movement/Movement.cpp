#include "Movement.h"
#include "../../../Engine/Input/Input.h"
#include "../../../Engine/Time/Time.h"
#include "../../Types/GameObject/GameObject.h"

Movement::Movement()
: ObjectComponent(0) {

}

Movement::Movement(const MovementParameters& params)
: ObjectComponent(0), speed(params.speed) {

}

void Movement::Start() { }

void Movement::Update() {
    if(!cameraTransform || !rb) return;

    auto movementDirection = glm::vec3(0.0f);

    glm::vec3 forward = glm::normalize(cameraTransform->ToForwardVector());
    glm::vec3 right = glm::normalize(cameraTransform->ToRightVector());
    
    if(Input::GetKey(Input::Key::KeyW)) {
        movementDirection += forward;
    }
    if(Input::GetKey(Input::Key::KeyS)) {
        movementDirection -= forward;
    }
    if(Input::GetKey(Input::Key::KeyD)) {
        movementDirection += right;
    }
    if(Input::GetKey(Input::Key::KeyA)) {
        movementDirection -= right;
    }

    if(movementDirection != glm::vec3(0.0f)) {
        movementDirection = glm::normalize(movementDirection);
    }

    glm::vec3 linearVelocity = movementDirection * speed * Time::GetDeltaTime();
    GetGameObject()->GetComponent<Transform>()->position += linearVelocity;
}

void Movement::SetCameraTransform(Transform *transform) {
    cameraTransform = transform;
}