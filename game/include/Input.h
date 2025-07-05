#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Input {
    public:
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mod);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};