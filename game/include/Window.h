#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);

    bool shouldClose();
    void swapBuffers();
    
    GLFWwindow* getNativeWindow() const { return window; }
    float getScreenRatio() const { return m_screenRatio; }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* window;
    float m_screenRatio = 1.0f;
};