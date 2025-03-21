#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Utils/Utils.h"

namespace Core {
    static constexpr int DEF_SCREEN_WIDTH = 800;
    static constexpr int DEF_SCREEN_HEIGHT = 600;

    void Initialize(int width = DEF_SCREEN_WIDTH, int height = DEF_SCREEN_HEIGHT,
                    const char *title = "OpenGL Program", bool fullscreen = false);
    void CreateWindow(int width, int height, bool fullscreen);
    NODISCARD bool WindowShouldClose();
    NODISCARD GLFWwindow *GetActiveWindow();
    NODISCARD float GetAspectRatio();
    void FinishFrame();
    void Dispose();
}