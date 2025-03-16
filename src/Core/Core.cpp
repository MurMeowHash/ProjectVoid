#include "Core.h"
#include "../Debug/Debug.h"
#include "../Error/Error.h"

namespace Core {
    GLFWwindow *mainWindow;
    int screenWidth;
    int screenHeight;
    const char *windowTitle;

    void Initialize(int width, int height, const char *title, bool fullscreen) {
        windowTitle = title;

        glfwInit();
        glfwSetErrorCallback([](int code, const char *description)
        {
            Debug::LogError(description);
        });
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        CreateWindow(width, height, fullscreen);

        glfwMakeContextCurrent(mainWindow);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Error::FallWithFatalError("GLAD", "Failed to load GL functions");
        }
    }

    void CreateWindow(int width, int height, bool fullscreen) {
        GLFWmonitor *targetMonitor;
        if(fullscreen) {
            targetMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *monitorProperties = glfwGetVideoMode(targetMonitor);
            screenWidth = monitorProperties->width;
            screenHeight = monitorProperties->height;
        } else {
            targetMonitor = nullptr;
            screenWidth = width;
            screenHeight = height;
        }

        mainWindow = glfwCreateWindow(screenWidth, screenHeight, windowTitle, targetMonitor, nullptr);
        if(!mainWindow) {
            Error::FallWithFatalError("GLFW", "Failed to create window");
        }
    }

    bool WindowShouldClose() {
        return glfwWindowShouldClose(mainWindow);
    }

    GLFWwindow *GetActiveWindow() {
        return mainWindow;
    }

    void FinishFrame() {
        glfwPollEvents();
        glfwSwapBuffers(mainWindow);
    }

    void Terminate() {
        glfwTerminate();
    }
}