#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

using namespace std;

class Window {
    public:
        Window(int width, int height, const string& title);

        bool shouldClose() { 
            return glfwWindowShouldClose(window); 
        }

        void swapBuffers() { 
            glfwSwapBuffers(window); 
        }

    private:
        GLFWwindow* window;
};