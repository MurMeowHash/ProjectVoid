#include "Time.h"
#include <GLFW/glfw3.h>

namespace Time {
    float prevTime = 0.0f;
    float deltaTime;

    void UpdateDeltaTime() {
        auto currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - prevTime;
        prevTime = currentTime;
    }

    float GetDeltaTime() {
        return deltaTime;
    }
}