#include "Game.h"
#include <iostream>
#include <stdexcept>

using namespace std;

int main() {
    try {
        Game game;
        game.run();
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    } catch (...) {
        cout << "Bah, deu ruim mesmo." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}