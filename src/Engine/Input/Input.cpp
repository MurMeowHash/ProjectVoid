#include "Input.h"
#include <GLFW/glfw3.h>
#include <cstring>
#include "../../Core/Core.h"

namespace Input {
    static constexpr int KEYS_COUNT = 349;
    static constexpr int MOUSE_BUTTON_COUNT = 8;
    static constexpr unsigned int STATIC_BEHAVIOR_MARKER = 1;

    int curKeysStates[KEYS_COUNT];
    int prevKeysStates[KEYS_COUNT];
    int curMouseButtonStates[MOUSE_BUTTON_COUNT];
    int prevMouseButtonStates[MOUSE_BUTTON_COUNT];
    unsigned int keyStaticBehaviorCount = 0;
    unsigned int mouseStaticBehaviorCount = 0;
    float scrollOffset;
    double cursorPosX;
    double cursorPosY;
    float cursorOffsetX;
    float cursorOffsetY;
    GLFWwindow *activeWindow;

    void key_callback(UNUSED GLFWwindow *window, int key, UNUSED int scancode, int action, UNUSED int mods);
    void mouse_callback(UNUSED GLFWwindow* window, int button, int action, UNUSED int mods);
    void scroll_callback(UNUSED GLFWwindow* window, UNUSED double xoffset, double yoffset);

    void SetCallbacks();
    void InitInputStates();
    void CopyInputStates(int *dest, int *src, int count, unsigned int &staticBehaviorCount);
    NODISCARD bool GetInputDown(const int *prevInputStates, const int *curInputStates, int input);
    NODISCARD bool GetInputPressed(const int *curInputStates, int input);
    NODISCARD bool GetInputUp(const int *prevInputStates, const int *curInputStates, int input);
    void CalcCursorOffset();

    void key_callback(UNUSED GLFWwindow *window, int key, UNUSED int scancode, int action, UNUSED int mods) {
        if(action == GLFW_REPEAT) return;

        curKeysStates[key] = action;
        keyStaticBehaviorCount = 0;
    }

    void mouse_callback(UNUSED GLFWwindow* window, int button, int action, UNUSED int mods) {
        if(action == GLFW_REPEAT) return;

        curMouseButtonStates[button] = action;
        mouseStaticBehaviorCount = 0;
    }

    void scroll_callback(UNUSED GLFWwindow* window, UNUSED double xoffset, double yoffset) {
        scrollOffset = static_cast<float>(yoffset);
    }

    void SetCallbacks() {
        glfwSetKeyCallback(activeWindow, key_callback);
        glfwSetMouseButtonCallback(activeWindow, mouse_callback);
        glfwSetScrollCallback(activeWindow, scroll_callback);
    }

    void InitInputStates() {
        glfwGetCursorPos(activeWindow, &cursorPosX, &cursorPosY);

        std::memset(curKeysStates, GLFW_RELEASE, KEYS_COUNT * sizeof(int));
        std::memset(curMouseButtonStates, GLFW_RELEASE, MOUSE_BUTTON_COUNT * sizeof(int));
        Update();
    }

    void CopyInputStates(int *dest, int *src, int count, unsigned int &staticBehaviorCount) {
        if(staticBehaviorCount != STATIC_BEHAVIOR_MARKER) {
            std::memcpy(dest, src, count * sizeof(int));
            ++staticBehaviorCount;
        }
    }

    bool GetInputDown(const int *prevInputStates, const int *curInputStates, int input) {
        return curInputStates[input] == GLFW_PRESS && prevInputStates[input] == GLFW_RELEASE;
    }

    bool GetInputPressed(const int *curInputStates, int input) {
        return curInputStates[input] == GLFW_PRESS;
    }

    bool GetInputUp(const int *prevInputStates, const int *curInputStates, int input) {
        return curInputStates[input] == GLFW_RELEASE && prevInputStates[input] == GLFW_PRESS;
    }

    void CalcCursorOffset() {
        double xPos, yPos;
        glfwGetCursorPos(activeWindow, &xPos, &yPos);
        if(Core::GetUsingMode() == Core::UsingMode::Game) {
        cursorOffsetX = static_cast<float>(xPos - cursorPosX);
        cursorOffsetY = static_cast<float>(cursorPosY - yPos);
        }
        else if(Core::GetUsingMode() == Core::UsingMode::UI && GetMouseButton(MouseButton::MouseLeft)) {
            if(xPos >= 0 && xPos <= Core::GetScreenWidth() - 400 && yPos >= 0 && yPos <= Core::GetScreenHeight() - 400) {
                cursorOffsetX = static_cast<float>(cursorPosX - xPos);
                cursorOffsetY = static_cast<float>(yPos - cursorPosY);
            }
        }
        cursorPosX = xPos;
        cursorPosY = yPos;
    }

    void Initialize() {
        activeWindow = Core::GetActiveWindow();
        InitInputStates();
        SetCallbacks();
    }

    void Update() {
        CopyInputStates(prevKeysStates, curKeysStates, KEYS_COUNT, keyStaticBehaviorCount);
        CopyInputStates(prevMouseButtonStates, curMouseButtonStates, MOUSE_BUTTON_COUNT, mouseStaticBehaviorCount);

        CalcCursorOffset();
        scrollOffset = 0.0f;
    }

    bool GetKeyDown(Key key) {
        return GetInputDown(prevKeysStates, curKeysStates, static_cast<int>(key));
    }

    bool GetKey(Key key) {
        return GetInputPressed(curKeysStates, static_cast<int>(key));
    }

    bool GetKeyUp(Key key) {
        return GetInputUp(prevKeysStates, curKeysStates, static_cast<int>(key));
    }

    bool GetMouseButtonDown(MouseButton mouseButton) {
        return GetInputDown(prevMouseButtonStates, curMouseButtonStates, static_cast<int>(mouseButton));
    }

    bool GetMouseButton(MouseButton mouseButton) {
        return GetInputPressed(curMouseButtonStates, static_cast<int>(mouseButton));
    }

    bool GetMouseButtonUp(MouseButton mouseButton) {
        return GetInputUp(prevMouseButtonStates, curMouseButtonStates, static_cast<int>(mouseButton));
    }

    float GetScrollOffset() {
        return scrollOffset;
    }

    float GetMouseOffsetX() {
        return cursorOffsetX;
    }

    float GetMouseOffsetY() {
        return cursorOffsetY;
    }

    unsigned int GetMousePosX() {
        return static_cast<unsigned int>(cursorPosX);
    }

    unsigned int GetMousePosY() {
        return static_cast<unsigned int>(cursorPosY);
    }

    void SetCursorLock(bool lock) {
        glfwSetInputMode(activeWindow, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }


    bool IsControlPressed() {
        return GetKey(Key::KeyLeftControl) || GetKey(Key::KeyRightControl);
    }

    bool IsShiftPressed() {
        return GetKey(Key::KeyLeftShift) || GetKey(Key::KeyRightShift);
    }
}