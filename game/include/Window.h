#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    // construtor para uma janela com tamanho e titulos definidos
    Window(int width, int height, const std::string& title);

    // função para verificar se o usuário quer fechar a janela, encerra o loop principal
    bool shouldClose();

    // 
    void swapBuffers();
    
    // getter para o ponteiro da window, o game usa isso para, por exemplo, tratar o input
    GLFWwindow* getNativeWindow() const { return window; }
    
    float getScreenRatio() const { return m_screenRatio; }

    // callback para tratar quando a janela é redimensionda.
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* window;
    float m_screenRatio = 1.0f;
};