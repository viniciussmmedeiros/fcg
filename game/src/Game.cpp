#include "Game.h"
#include <iostream>

using namespace std;

// Quando um objeto de 'Game' é criado, ele também cria um objeto
// de 'Window' para o membro 'window'
Game::Game() : window(800, 800, "Escape da Vaca") {
    init();
}

// Número de texturas carregadas pela função LoadTextureImage()
GLuint g_NumLoadedTextures = 0;

void Game::init() {
    glfwSetWindowUserPointer(window.getNativeWindow(), this);

    glfwSetKeyCallback(window.getNativeWindow(), Input::KeyCallback);
    glfwSetMouseButtonCallback(window.getNativeWindow(), Input::MouseButtonCallback);
    glfwSetCursorPosCallback(window.getNativeWindow(), Input::CursorPosCallback);
    glfwSetScrollCallback(window.getNativeWindow(), Input::ScrollCallback);

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
    // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
    // Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    //
    //           R     G     B     A
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    cowPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    texture = std::unique_ptr<Texture>(new Texture());
    texture->LoadTextureImage("../../data/cow_surface.jpg", g_NumLoadedTextures);
    g_NumLoadedTextures += 1;

    try {
        shader.reset(new Shader("../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl"));
        // models["sphere"].reset(new Model("../../data/sphere.obj"));
        // models["bunny"].reset(new Model("../../data/bunny.obj"));
        // models["plane"].reset(new Model("../../data/plane.obj"));
        models["cow"].reset(new Model("../../data/cow.obj"));
    } catch (exception& e) {
        fprintf(stderr, "Error opening data resources \"%s\".\n", e.what());
    }
}

void Game::run() {
    float lastFrameT = 0.0f;

    while (!window.shouldClose()) {
        float currentFrameT = glfwGetTime();
        float deltaT = currentFrameT - lastFrameT;
        // usar deltaT para fazer as atualizações no jogo, animações etc
        lastFrameT = currentFrameT;
        render();
        window.swapBuffers(); // atualiza a tela
        glfwPollEvents(); // processa eventos tipo input
    }
}

void Game::render() {
    // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
    // e também resetamos todos os pixels do Z-buffer (depth buffer).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();

    if(camera.getUseFreeCamera()) {
        // glm::mat4 cowOrientation = Matrix_Rotate_Z(cowAngleZ) *
        //                     Matrix_Rotate_Y(cowAngleY) *
        //                     Matrix_Rotate_X(cowAngleX);

        // glm::vec3 cowForward(1.0f, 0.0f, 0.0f);
        // float cameraDistance = 2.0f;
        // glm::vec3 cameraOffset = glm::vec3(cowOrientation * glm::vec4(-cowForward, 0.0f));
        // glm::vec3 currFreePosition = cowPosition + cameraOffset * cameraDistance;

        // float cameraHeight = 1.0f;
        // currFreePosition += glm::vec3(0.0f, cameraHeight, 0.0f);

        // camera.setPosition(currFreePosition);
    }
    glm::mat4 viewMatrix = camera.getVirtualCamera();

    glm::mat4 projectionMatrix = camera.getProjectionMatrix(window.getScreenRatio());

    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);
    
    // Desenha a esfera
    // glm::mat4 modelMatrix = Matrix_Translate(-1.0f, 0.0f, 0.0f);
    // shader->setMat4("model", modelMatrix);
    // shader->setInt("object_id", 0);
    // models["sphere"]->draw();

    // Desenha o coelho
    // modelMatrix = Matrix_Translate(1.0f, 0.0f, 0.0f)
    //             * Matrix_Rotate_Z(bunnyAngleZ)
    //             * Matrix_Rotate_Y(bunnyAngleY)
    //             * Matrix_Rotate_X(bunnyAngleX);
    // shader->setMat4("model", modelMatrix);
    // shader->setInt("object_id", 1);
    // models["bunny"]->draw();

    // Desenha o plano
    // modelMatrix = Matrix_Translate(0.0f, -1.0f, 0.0f) * Matrix_Scale(2.0f, 1.0f, 2.0f);
    // shader->setMat4("model", modelMatrix);
    // shader->setInt("object_id", 2);
    // models["plane"]->draw();

    // desenha a vaca
    glm::mat4 modelMatrix = Matrix_Translate(cowPosition.x, cowPosition.y, cowPosition.z);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 3);
    models["cow"]->draw();
}

void Game::handleKeyCallback(int key, int scancode, int action, int mod) {
    if (action == GLFW_PRESS) {
        // Se o usuário pressionar a tecla ESC, fechamos a janela.
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window.getNativeWindow(), true);
        }
        // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
        if (key == GLFW_KEY_P) {
            camera.setPerspective(true);
        }
        // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
        if (key == GLFW_KEY_O) {
            camera.setPerspective(false);
        }
        
        float delta = 3.141592 / 16.0f;
        // if (key == GLFW_KEY_X) {
        //     bunnyAngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
        // }
        // if (key == GLFW_KEY_Y) {
        //     bunnyAngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
        // }
        // if (key == GLFW_KEY_Z) {
        //     bunnyAngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
        // }

        glm::mat4 cowOrientation = Matrix_Rotate_Z(cowAngleZ) *
                                   Matrix_Rotate_Y(cowAngleY) *
                                   Matrix_Rotate_X(cowAngleX);

        glm::vec3 cowForward(1.0f, 0.0f, 0.0f); // +x é para frente
        glm::vec3 cowRight(0.0f, 0.0f, 1.0f); // +z é direita

        if (key == GLFW_KEY_V) {
            camera.setFreeCamera(!camera.getUseFreeCamera());

            if(camera.getUseFreeCamera()) {
                // float cameraDistance = 2.0f;
                // glm::vec3 cameraOffset = glm::vec3(cowOrientation * glm::vec4(-cowForward, 0.0f));
                // glm::vec3 currFreePosition = cowPosition + cameraOffset * cameraDistance;

                // float altura = 1.0f;
                // currFreePosition += glm::vec3(0.0f, altura, 0.0f);

                // glm::vec3 toCow = glm::normalize(cowPosition - currFreePosition);
                // float theta = atan2(toCow.x, toCow.z);
                // float phi = -asin(toCow.y);

                // camera.setCameraTheta(theta);
                // camera.setCameraPhi(phi);

                camera.setCameraPhi(0);
                camera.setCameraTheta(4.141592f);
                // camera.setPosition(cowPosition);
            }
        }

        if (key == GLFW_KEY_W) {
            glm::vec3 forwardVec = glm::vec3(cowOrientation * glm::vec4(cowForward, 0.0f));
            cowPosition += forwardVec * 0.1f;
        }
        if (key == GLFW_KEY_S) {
            glm::vec3 backwardVec = glm::vec3(cowOrientation * glm::vec4(-cowForward, 0.0f));
            cowPosition += backwardVec * 0.1f;
        }
        if (key == GLFW_KEY_A) {
            glm::vec3 leftVec = glm::vec3(cowOrientation * glm::vec4(-cowRight, 0.0f));
            cowPosition += leftVec * 0.1f;
        }
        if (key == GLFW_KEY_D) {
            glm::vec3 rightVec = glm::vec3(cowOrientation * glm::vec4(cowRight, 0.0f));
            cowPosition += rightVec * 0.1f;
        }
    }
}

void Game::handleMouseButton(int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
            // posição atual do cursor nas variáveis g_LastCursorPosX e
            // g_LastCursorPosY.  Também, setamos a variável
            // g_LeftMouseButtonPressed como true, para saber que o usuário está
            // com o botão esquerdo pressionado.
            leftMouseButtonPressed = true;
            glfwGetCursorPos(window.getNativeWindow(), &lastCursorPosX, &lastCursorPosY);
        } else if (action == GLFW_RELEASE) {
            // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
            // variável abaixo para false.
            leftMouseButtonPressed = false;
        }
    }
}

void Game::handleCursorPos(double xpos, double ypos) {
    if (leftMouseButtonPressed) {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - lastCursorPosX;
        float dy = ypos - lastCursorPosY;
        
        // Atualizamos parâmetros da câmera com os deslocamentos
        camera.cursorPosCallback(dx, dy);
        
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        lastCursorPosX = xpos;
        lastCursorPosY = ypos;
    }
}

void Game::handleScroll(double xoffset, double yoffset) {
    camera.scrollCallback(yoffset);
}

// glm::vec4 getFreeCameraPosition() {
    
// }