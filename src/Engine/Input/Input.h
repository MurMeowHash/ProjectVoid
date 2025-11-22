#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../Utils/Utils.h"

namespace Input {
    enum class Key {
        KeyEscape = GLFW_KEY_ESCAPE,
        KeyA = GLFW_KEY_A,
        KeyD = GLFW_KEY_D,
        KeyS = GLFW_KEY_S,
        KeyW = GLFW_KEY_W,
        KeyF = GLFW_KEY_F,
        KeySpace = GLFW_KEY_SPACE
    };

    enum class MouseButton {
        MouseLeft = GLFW_MOUSE_BUTTON_LEFT,
        MouseRight = GLFW_MOUSE_BUTTON_RIGHT,
        MouseWheel = GLFW_MOUSE_BUTTON_MIDDLE
    };

    void Initialize();
    void Update();
    NODISCARD bool GetKeyDown(Key key);
    NODISCARD bool GetKey(Key key);
    NODISCARD bool GetKeyUp(Key key);
    NODISCARD bool GetMouseButtonDown(MouseButton mouseButton);
    NODISCARD bool GetMouseButton(MouseButton mouseButton);
    NODISCARD bool GetMouseButtonUp(MouseButton mouseButton);
    NODISCARD float GetScrollOffset();
    NODISCARD float GetMouseOffsetX();
    NODISCARD float GetMouseOffsetY();
    NODISCARD unsigned int GetMousePosX();
    NODISCARD unsigned int GetMousePosY();
    void SetCursorLock(bool lock);
}