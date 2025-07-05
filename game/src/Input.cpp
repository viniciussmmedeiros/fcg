#include "Input.h"
#include "Game.h"

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod) {
    // em game.cpp setamos o pointer como o game, aqui "pegamos de volta" fazendo um cast
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) {
        // agora que temos de volta o ponteiro para game, chamamos o método apropriado, passando os dados do evento
        game->handleKeyCallback(key, scancode, action, mod);
    }
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mod) {
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) {
        game->handleMouseButton(button, action, mod);
    }
}

void Input::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) {
        game->handleCursorPos(xpos, ypos);
    }
}