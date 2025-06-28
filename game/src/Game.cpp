#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stb_image.h>

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

    cowPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    try {
        shader.reset(new Shader("../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl"));

        textures["earth"] = std::unique_ptr<Texture>(new Texture());
        textures["earth"]->LoadTextureImage("../../data/tc-earth_daymap_surface.jpg", g_NumLoadedTextures++);
        textures["cow"] = std::unique_ptr<Texture>(new Texture());
        textures["cow"]->LoadTextureImage("../../data/cow_surface.jpg", g_NumLoadedTextures++);
        textures["bunny"] = std::unique_ptr<Texture>(new Texture());
        textures["bunny"]->LoadTextureImage("../../data/bunny_surface.jpg", g_NumLoadedTextures);

        models["sphere"].reset(new Model("../../data/sphere.obj"));
        models["bunny"].reset(new Model("../../data/bunny.obj"));
        models["plane"].reset(new Model("../../data/plane.obj"));
        models["cow"].reset(new Model("../../data/cow.obj"));
    } catch (exception& e) {
        fprintf(stderr, "Error opening data resources \"%s\".\n", e.what());
    }

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
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
    printf("\n[TESTES]\n\tT Y U I -> movimentam a câmera livre\n\tW A S D -> movimentam a vaca\n\tV -> troca entre câmera livre e look-at\n");
    // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
    // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
    // Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    //
    //           R     G     B     A
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
    // e também resetamos todos os pixels do Z-buffer (depth buffer).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 viewMatrix = camera.getVirtualCamera();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix(window.getScreenRatio());

    shader->use();
    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);

    GLint g_bbox_min_uniform = shader->getBBoxMinUniform();
    GLint g_bbox_max_uniform = shader->getBBoxMaxUniform();

    // desenha a esfera
    textures["earth"]->bind();
    shader->setInt("TextureImage0", textures["earth"]->getTextureUnit());
    glm::mat4 modelMatrix = Matrix_Translate(-1.0f, 0.0f, 0.0f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 0);
    models["sphere"]->draw("the_sphere", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["earth"]->unbind();

    // Desenha o coelho
    textures["bunny"]->bind();
    shader->setInt("TextureImage0", textures["bunny"]->getTextureUnit());
    modelMatrix = Matrix_Translate(1.0f, 0.0f, 0.0f)
                * Matrix_Rotate_Z(bunnyAngleZ)
                * Matrix_Rotate_Y(bunnyAngleY)
                * Matrix_Rotate_X(bunnyAngleX);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 1);
    models["bunny"]->draw("the_bunny", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["bunny"]->unbind();

    // Desenha o plano
    textures["earth"]->bind();
    shader->setInt("TextureImage0", textures["earth"]->getTextureUnit());
    modelMatrix = Matrix_Translate(0.0f, -1.0f, 0.0f) * Matrix_Scale(2.0f, 1.0f, 2.0f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 2);
    models["plane"]->draw("the_plane", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["earth"]->unbind();

    // desenha a vaca
    textures["cow"]->bind();
    shader->setInt("TextureImage0", textures["cow"]->getTextureUnit());
    modelMatrix = Matrix_Translate(cowPosition.x, cowPosition.y, cowPosition.z);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 3);
    models["cow"]->draw("the_cow", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["cow"]->unbind();
}

void Game::handleKeyCallback(int key, int scancode, int action, int mod) {
    if (action == GLFW_PRESS) {
        // Se o usuário pressionar a tecla ESC, fechamos a janela.
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window.getNativeWindow(), true);
        }
        
        float delta = 3.141592 / 16.0f;
        
        glm::mat4 cowOrientation = Matrix_Rotate_Z(cowAngleZ) *
                                   Matrix_Rotate_Y(cowAngleY) *
                                   Matrix_Rotate_X(cowAngleX);

        glm::vec3 cowForward(1.0f, 0.0f, 0.0f); // +x é para frente
        glm::vec3 cowRight(0.0f, 0.0f, 1.0f); // +z é direita

        if (key == GLFW_KEY_V) {
            camera.setFreeCamera(!camera.getUseFreeCamera());
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

        float speed = 0.85f;
        glm::vec4 vector_u = crossproduct(glm::vec4(0.0f,1.0f,0.0f,0.0f), camera.getCameraViewVector());
        vector_u = normalize(vector_u);
        if(key == GLFW_KEY_T) {
            glm::vec4 new_camera_position = camera.getFreeCameraPosition() += speed * camera.getCameraViewVector();
            camera.setFreeCameraPosition(new_camera_position);
        }
        
        if(key == GLFW_KEY_Y) {
            glm::vec4 new_camera_position = camera.getFreeCameraPosition() -= speed * camera.getCameraViewVector();
            camera.setFreeCameraPosition(new_camera_position);
        }

        if(key == GLFW_KEY_U) {
            glm::vec4 new_camera_position = camera.getFreeCameraPosition() += speed * vector_u;
            camera.setFreeCameraPosition(new_camera_position);
        }

        if(key == GLFW_KEY_I) {
            glm::vec4 new_camera_position = camera.getFreeCameraPosition() -= speed * vector_u;
            camera.setFreeCameraPosition(new_camera_position);
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