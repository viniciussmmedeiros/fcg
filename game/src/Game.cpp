#include "Game.h"

// Quando um objeto de 'Game' é criado, ele também cria um objeto
// de 'Window' para o membro 'window'
Game::Game() : window(800, 800, "Escape da Vaca") {}

void Game::run() {
    while (!window.shouldClose()) {
        window.swapBuffers(); // atualiza a tela
        glfwPollEvents(); // processa eventos tipo input
    }
}