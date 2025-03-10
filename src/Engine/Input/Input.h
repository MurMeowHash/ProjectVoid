#pragma once
#include "../../Utils/Utils.h"
#include "InputKey.h"
#include "InputMouseButton.h"

namespace Input {
    void Initialize();
    void Update();
    NODISCARD bool GetKeyDown(InputKey key);
    NODISCARD bool GetKey(InputKey key);
    NODISCARD bool GetKeyUp(InputKey key);
    NODISCARD bool GetMouseButtonDown(InputMouseButton mouseButton);
    NODISCARD bool GetMouseButton(InputMouseButton mouseButton);
    NODISCARD bool GetMouseButtonUp(InputMouseButton mouseButton);
    NODISCARD float GetScrollOffset();
    NODISCARD float GetMouseOffsetX();
    NODISCARD float GetMouseOffsetY();
    NODISCARD unsigned int GetMousePosX();
    NODISCARD unsigned int GetMousePosY();
    void SetCursorLock(bool lock);
}