#include "Movement.h"
#include "../../../Engine/Input/Input.h"
#include "../../Types/GameObject/GameObject.h"
#include "../../../Engine/Time/Time.h"
#include "../../../Debug/Debug.h"
#include "../ComponentMacros.h"
#include "../../../Utils/JsonUtils.h"
#include <nlohmann/json.hpp>

Movement::Movement()
: ObjectComponent(0) {

}

Movement::Movement(const MovementParameters& params)
: ObjectComponent(0), speed(params.speed) {

}


void Movement::Start() {
    rb = GetGameObject()->GetComponent<Rigidbody>();
}

void Movement::Update() {
    if(!cameraTransform) return;

    auto movementDirection = glm::vec3(0.0f);
    if(Input::GetKey(Input::Key::KeyW)) {
        movementDirection += cameraTransform->ToForwardVector();
    }
    if(Input::GetKey(Input::Key::KeyS)) {
        movementDirection -= cameraTransform->ToForwardVector();
    }
    if(Input::GetKey(Input::Key::KeyD)) {
        movementDirection += cameraTransform->ToRightVector();
    }
    if(Input::GetKey(Input::Key::KeyA)) {
        movementDirection -= cameraTransform->ToRightVector();
    }

    if(movementDirection != glm::vec3(0.0f)) {
        movementDirection = glm::normalize(movementDirection);
    }

    glm::vec3 linearVelocity = movementDirection * speed;
    linearVelocity.y = rb->GetLinearVelocity().y;
    rb->SetLinearVelocity(linearVelocity);
}

void Movement::SetCameraTransform(Transform *transform) {
    cameraTransform = transform;
}

Movement* Movement::CreateFromJson(GameObject* owner, const nlohmann::json& params) {
    auto movementParams = MovementParameters();
    SetIfExists(params, "speed", movementParams.speed);
    return owner->AddComponent<Movement>(movementParams);
}

nlohmann::json Movement::SerializeToJson() const {
    nlohmann::json params;
    params["speed"] = speed;
    return params;
}

REGISTER_COMPONENT_FROM_JSON(Movement)