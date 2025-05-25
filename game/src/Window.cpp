#include "Window.h"
#include <stdexcept>
#include <cstdio>

void ErrorCallback(int error, const char* description) {
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }
    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("ERROR: glfwCreateWindow() failed.");
    }
    
    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetWindowUserPointer(window, this);
    
    framebufferSizeCallback(window, width, height);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::framebufferSizeCallback(GLFWwindow* nativeWindow, int width, int height) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(nativeWindow));
    if (window) {
        glViewport(0, 0, width, height);
        if (height > 0) {
            window->m_screenRatio = (float)width / height;
        }
    }
}