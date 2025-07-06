#pragma once
#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Input.h"
#include "Texture.h"
#include "collisions.h"
#include "TextRenderer.h" 
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
        void updateSphereAnimation(float deltaTime);
        bool checkCowMovementAndPushBoxes(const glm::vec3& newCowPos, const glm::vec3& movementDirection);

    private:
        void init();
        void render();
        void updateGameState();

        Window window;
        Camera camera;
        std::unique_ptr<Shader> shader;
        
        std::map<std::string, std::unique_ptr<Texture>> textures;
        std::map<std::string, std::unique_ptr<Model>> models;

        std::vector<glm::vec3> boxColors;
        std::vector<glm::vec3> tilePositions;

        std::vector<bool> boxInPlace;
        bool gameWon = false;
        // long unsigned int, pois sendo 'int' o compilador gera warning na comparação com boxPositions.size()
        long unsigned int correctPlacementsCount = 0;
        void renderWinMessage();
        
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
        glm::mat4 cowOrientation;

        // Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
        float bunnyAngleX = 0.0f;
        float bunnyAngleY = 0.0f;
        float bunnyAngleZ = 0.0f;

        // Variáveis para animação da esfera com curva de Bézier
        float sphereAnimationTime = 0.0f;
        float sphereAnimationDuration = 5.0f;
        glm::vec3 sphereBezierP0, sphereBezierP1, sphereBezierP2, sphereBezierP3;

        // Posições dinâmicas das caixas que podem ser empurradas
        std::vector<glm::vec3> boxPositions;
        float boxSize = 1.0f;

        bool keyWPressed = false;
        bool keyAPressed = false;
        bool keySPressed = false;
        bool keyDPressed = false;

        std::vector<Collisions::AABB> worldObstacles;

        bool keyPressed[1024] = {false};

        bool showInfoText = true;
        void renderInfoText();
        void renderPermanentText();
        void renderFPS();
};