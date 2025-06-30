#pragma once
#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Input.h"
#include "Texture.h"
#include "Collisions.h"
#include <memory>
#include <map>

class Game {
    public:
        Game();

        void run();

        void handleKeyCallback(int key, int scancode, int action, int mod);
        void handleMouseButton(int button, int action, int mod);
        void handleCursorPos(double xpos, double ypos);
        void processCowMovement(float deltaTime);

    private:
        void init();
        void render();

        Window window;
        Camera camera;
        std::unique_ptr<Shader> shader;
        
        std::map<std::string, std::unique_ptr<Texture>> textures;
        std::map<std::string, std::unique_ptr<Model>> models;

        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.s
        bool leftMouseButtonPressed = false;

        // Variáveis globais que armazenam a última posição do cursor do mouse, para
        // que possamos calcular quanto que o mouse se movimentou entre dois instantes
        // de tempo. Utilizadas no callback CursorPosCallback() abaixo.
        double lastCursorPosX = 0.0, lastCursorPosY = 0.0;
        
        float cowAngleX = 0.0f;
        float cowAngleY = 0.0f;
        float cowAngleZ = 0.0f;
        glm::vec3 cowPosition;

        // Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
        float bunnyAngleX = 0.0f;
        float bunnyAngleY = 0.0f;
        float bunnyAngleZ = 0.0f;

        bool keyWPressed = false;
        bool keyAPressed = false;
        bool keySPressed = false;
        bool keyDPressed = false;

        std::vector<Collisions::AABB> worldObstacles;

        bool keyPressed[1024] = {false};
};