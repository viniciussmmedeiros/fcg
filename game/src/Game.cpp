#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stb_image.h>

using namespace std;

Game::Game() : window(800, 800, "Empurra Vaca!") {
    init();
}

glm::vec3 bezierCubic(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
    return float(std::pow(1 - t, 3)) * p0 +
           3.0f * t * float(std::pow(1 - t, 2)) * p1 +
           3.0f * float(std::pow(t, 2)) * (1 - t) * p2 +
           float(std::pow(t, 3)) * p3;
}

// FONTE: Laboratórios - Número de texturas carregadas pela função LoadTextureImage()
GLuint g_NumLoadedTextures = 0;

void Game::init() {
    glfwSetWindowUserPointer(window.getNativeWindow(), this);

    glfwSetKeyCallback(window.getNativeWindow(), Input::KeyCallback);
    glfwSetMouseButtonCallback(window.getNativeWindow(), Input::MouseButtonCallback);
    glfwSetCursorPosCallback(window.getNativeWindow(), Input::CursorPosCallback);

    cowPosition = glm::vec3(0.0f, -0.4f, 0.0f);

    // Definições da curva de Bézier
    sphereAnimationTime = 0.0f;
    sphereAnimationDuration = 5.0f;
    
    sphereBezierP0 = glm::vec3(-3.0f, 0.5f, -3.0f);
    sphereBezierP1 = glm::vec3(-1.0f, 1.5f, 3.0f);
    sphereBezierP2 = glm::vec3(1.0f, 1.5f, -3.0f);
    sphereBezierP3 = glm::vec3(3.0f, 0.5f, 3.0f);

    // Posição inicial das caixas
    boxPositions = {
        glm::vec3(2.0f, -0.4f, 2.0f),
        glm::vec3(-2.0f, -0.4f, -2.0f),
        glm::vec3(1.0f, -0.4f, -3.0f)
    };

    boxInPlace.resize(boxPositions.size(), false);
    boxColors.push_back(glm::vec3(1.0f, 0.2f, 0.2f));
    boxColors.push_back(glm::vec3(0.2f, 1.0f, 0.2f));
    boxColors.push_back(glm::vec3(0.2f, 0.2f, 1.0f));

    tilePositions.push_back(glm::vec3(4.0f, -0.99f, 4.0f));
    tilePositions.push_back(glm::vec3(-4.0f, -0.99f, 4.0f));
    tilePositions.push_back(glm::vec3(0.0f, -0.99f, -4.0f));

    try {
        // FONTE: Laboratórios

        shader.reset(new Shader("../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl"));

        textures["earth"] = std::unique_ptr<Texture>(new Texture());
        textures["earth"]->LoadTextureImage("../../data/tc-earth_daymap_surface.jpg", g_NumLoadedTextures++);
        textures["cow"] = std::unique_ptr<Texture>(new Texture());
        textures["cow"]->LoadTextureImage("../../data/cow_surface.jpg", g_NumLoadedTextures++);
        textures["ceiling"] = std::unique_ptr<Texture>(new Texture());
        textures["ceiling"]->LoadTextureImage("../../data/ceiling_surface.jpg", g_NumLoadedTextures++);
        textures["floor"] = std::unique_ptr<Texture>(new Texture());
        textures["floor"]->LoadTextureImage("../../data/floor_surface.jpg", g_NumLoadedTextures++);
        textures["wall"] = std::unique_ptr<Texture>(new Texture());
        textures["wall"]->LoadTextureImage("../../data/wall_surface.jpg", g_NumLoadedTextures++);
        textures["box"] = std::unique_ptr<Texture>(new Texture());
        textures["box"]->LoadTextureImage("../../data/box_surface.jpg", g_NumLoadedTextures);

        models["sphere"].reset(new Model("../../data/sphere.obj"));
        models["plane"].reset(new Model("../../data/plane.obj"));
        models["cow"].reset(new Model("../../data/cow.obj"));
        models["cube"].reset(new Model("../../data/cube.obj"));
    } catch (exception& e) {
        fprintf(stderr, "Error opening data resources \"%s\".\n", e.what());
    }

    // FONTE - Laboratórios: Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // FONTE - Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    TextRendering_Init();
}

void Game::run() {
    float lastFrameT = 0.0f;

    while (!window.shouldClose()) {
        float currentFrameT = glfwGetTime();
        
        float deltaT = currentFrameT - lastFrameT;
        
        lastFrameT = currentFrameT;

        processCowMovement(deltaT);
        updateSphereAnimation(deltaT);
        updateGameState();

        render();
        renderInfoText();
        renderPermanentText();
        renderFPS(); 
        renderWinMessage();

        window.swapBuffers();
        glfwPollEvents();
    }
}

void Game::render() {
    // FONTE: Laboratórios - Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
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
    shader->setVec3("object_game_color", glm::vec3(1.0f, 1.0f, 1.0f));

    GLint g_bbox_min_uniform = shader->getBBoxMinUniform();
    GLint g_bbox_max_uniform = shader->getBBoxMaxUniform();
    textures["earth"]->bind();
    shader->setInt("TextureImage0", textures["earth"]->getTextureUnit());
    
    float t = sphereAnimationTime / sphereAnimationDuration;
    t = fmod(t, 1.0f); // garantir que t permaneça entre 0 e 1
    glm::vec3 testPoint = glm::vec3(0.20f, 1.25f, 0.0f);
    float sphereRadius = 0.3f; // o raio original da esfera é 1, mas aplicamos um 'matrix_scale' de 0.3
    glm::vec3 spherePosition = bezierCubic(sphereBezierP0, sphereBezierP1, sphereBezierP2, sphereBezierP3, t);

    bool hitPoint = Collisions::CheckPointSphereCollision(testPoint, spherePosition, sphereRadius);
    if(hitPoint) {
        shader->setVec3("object_game_color", glm::vec3(252.0f, 0, 0));
    }
    glm::mat4 modelMatrix = Matrix_Translate(spherePosition.x, spherePosition.y, spherePosition.z) * Matrix_Scale(0.3f, 0.3f, 0.3f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 0);
    models["sphere"]->draw("the_sphere", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["earth"]->unbind();
    shader->setVec3("object_game_color", glm::vec3(1.0f, 1.0f, 1.0f));
    // desenha a vaca
    textures["cow"]->bind();
    shader->setInt("TextureImage0", textures["cow"]->getTextureUnit());
    modelMatrix = Matrix_Translate(cowPosition.x, cowPosition.y, cowPosition.z) * cowOrientation;
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 3);
    models["cow"]->draw("the_cow", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["cow"]->unbind();

    // Desenha o chão
    textures["floor"]->bind();
    shader->setInt("TextureImage0", textures["floor"]->getTextureUnit());
    modelMatrix = Matrix_Translate(0.0f, -1.0f, 0.0f) * Matrix_Scale(5.0f, 0.1f, 5.0f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 4);
    models["plane"]->draw("the_plane", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["floor"]->unbind();

    // Parede esquerda
    textures["wall"]->bind();
    shader->setInt("TextureImage0", textures["wall"]->getTextureUnit());
    modelMatrix = Matrix_Translate(-5.1, 0.5f, 0.0f) 
            * Matrix_Scale(0.2f, 3.0f, 10.0f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 5);
    models["cube"]->draw("the_cube", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["wall"]->unbind();

    worldObstacles.push_back({
        glm::vec3(-5.1f, -1.0f, -5.0f),
        glm::vec3(-4.9f, 2.0f, 5.0f)
    });

    // Parede direita
    textures["wall"]->bind();
    shader->setInt("TextureImage0", textures["wall"]->getTextureUnit());
    modelMatrix = Matrix_Translate(5.1f, 0.5f, 0.0f) 
        * Matrix_Scale(0.2f, 3.0f, 10.0f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 2);
    models["cube"]->draw("the_cube", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["wall"]->unbind();

    worldObstacles.push_back({
        glm::vec3(4.9f, -1.0f, -5.0f),
        glm::vec3(5.1f, 2.0f, 5.0f)
    });

    // Parede frontal
    textures["wall"]->bind();
    shader->setInt("TextureImage0", textures["wall"]->getTextureUnit());
    modelMatrix = Matrix_Translate(0.0f, 0.5f, -5.1f) * Matrix_Scale(10.0f, 3.0f, 0.2f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 1);
    models["cube"]->draw("the_cube", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["wall"]->unbind();
    
    worldObstacles.push_back({
        glm::vec3(-5.0f, -1.0f, -5.1f),
        glm::vec3(5.0f, 2.0f, -4.9f)
    });
    
    // Parede de trás
    textures["wall"]->bind();
    shader->setInt("TextureImage0", textures["wall"]->getTextureUnit());
    modelMatrix = Matrix_Translate(0.0f, 0.5f, 5.1f) * Matrix_Scale(10.0f, 3.0f, 0.2f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 1);
    models["cube"]->draw("the_cube", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["wall"]->unbind();
    
    worldObstacles.push_back({
        glm::vec3(-5.0f, -1.0f, 4.9f),
        glm::vec3(5.0f, 2.0f, 5.1f)
    });
    
    textures["ceiling"]->bind();
    shader->setInt("TextureImage0", textures["ceiling"]->getTextureUnit());
    modelMatrix = Matrix_Translate(0.0f, 2.0f, 0.0f) 
                * Matrix_Scale(10.0f, 0.1f, 10.0f);
    shader->setMat4("model", modelMatrix);
    shader->setInt("object_id", 6);
    models["cube"]->draw("the_cube", g_bbox_min_uniform, g_bbox_max_uniform);
    textures["ceiling"]->unbind();

    for (size_t i = 0; i < boxPositions.size(); ++i) {
        textures["box"]->bind();
        shader->setInt("TextureImage0", textures["box"]->getTextureUnit());
        shader->setVec3("object_game_color", boxColors[i]);
        modelMatrix = Matrix_Translate(boxPositions[i].x, boxPositions[i].y, boxPositions[i].z) 
                    * Matrix_Scale(boxSize, boxSize, boxSize);

        shader->setMat4("model", modelMatrix);
        shader->setInt("object_id", 7);
        models["cube"]->draw("the_cube", g_bbox_min_uniform, g_bbox_max_uniform);
        textures["box"]->unbind();
    }

    shader->setVec3("object_game_color", glm::vec3(1.0f, 1.0f, 1.0f)); 
    textures["floor"]->bind();
    shader->setInt("TextureImage0", textures["floor"]->getTextureUnit());
    for (size_t i = 0; i < tilePositions.size(); ++i) {
        shader->setVec3("object_game_color", boxColors[i]);

        modelMatrix = Matrix_Translate(tilePositions[i].x, tilePositions[i].y, tilePositions[i].z)
                    * Matrix_Scale(0.8f, 0.01f, 0.8f);
        shader->setMat4("model", modelMatrix);
        models["plane"]->draw("the_plane", g_bbox_min_uniform, g_bbox_max_uniform);
    }

    worldObstacles.clear();
   
    worldObstacles.push_back({
        glm::vec3(-6.5f, -1.0f, -5.0f),
        glm::vec3(-4.9f, 2.0f, 5.0f)
    });
    worldObstacles.push_back({
        glm::vec3(6.5f, -1.0f, -5.0f),
        glm::vec3(5.1f, 2.0f, 5.0f)
    });
    worldObstacles.push_back({
        glm::vec3(-5.0f, -1.0f, -6.5f),
        glm::vec3(5.0f, 2.0f, -4.9f)
    });
    worldObstacles.push_back({
        glm::vec3(-5.0f, -1.0f, 6.5f),
        glm::vec3(5.0f, 2.0f, 5.1f)
    });

    for (const auto& pos : boxPositions) {
        worldObstacles.push_back({
            pos + glm::vec3(-boxSize/2.0f, 0.0f, -boxSize/2.0f),
            pos + glm::vec3(boxSize/2.0f, boxSize, boxSize/2.0f)
        });
    }
}

void Game::handleKeyCallback(int key, int scancode, int action, int mod) {
    if (action == GLFW_PRESS) {
        keyPressed[key] = true;
    }

    if (action == GLFW_RELEASE) {
        keyPressed[key] = false;
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

void Game::processCowMovement(float deltaTime) {
    if (keyPressed[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window.getNativeWindow(), true);
    }
    
    cowOrientation = Matrix_Rotate_Y(camera.getFreeCameraTheta() - M_PI / 2);

    glm::vec3 cowForward(1.0f, 0.0f, 0.0f); // +x é para frente
    glm::vec3 cowRight(0.0f, 0.0f, 1.0f); // +z é direita

    const float distance_from_cow = 2.0f;
    const float height_above_cow = 1.0f;
    glm::vec4 view_vector = camera.getCameraViewVector();
    glm::vec4 offset = -distance_from_cow * glm::normalize(view_vector);
    glm::vec4 new_camera_pos = glm::vec4(cowPosition, 1.0f) + offset;
    new_camera_pos.y += height_above_cow;

    camera.setFreeCameraPosition(new_camera_pos);

    if (keyPressed[GLFW_KEY_V]) {
        camera.setUseFreeCamera(!camera.getUseFreeCamera());
    }

    // Velocidade da vaca em unidades por segundo
    float speed_cow = 2.0f;

    if (keyPressed[GLFW_KEY_W]) {
        glm::vec3 forwardVec = glm::vec3(cowOrientation * glm::vec4(cowForward, 0.0f));
        glm::vec3 newPos = cowPosition + forwardVec * speed_cow * deltaTime;
    
        if (Collisions::CheckCowMovementAndPushBoxes(newPos, forwardVec, boxPositions, boxSize)) {
            cowPosition = newPos;
        }
    }

    if (keyPressed[GLFW_KEY_S]) {
        glm::vec3 backwardVec = glm::vec3(cowOrientation * glm::vec4(-cowForward, 0.0f));
        glm::vec3 newPos = cowPosition + backwardVec * speed_cow * deltaTime;
        
        if (Collisions::CheckCowMovementAndPushBoxes(newPos, backwardVec, boxPositions, boxSize)) {
            cowPosition = newPos;
        }
    }

    if (keyPressed[GLFW_KEY_A]) {
        glm::vec3 leftVec = glm::vec3(cowOrientation * glm::vec4(-cowRight, 0.0f));
        glm::vec3 newPos = cowPosition + leftVec * speed_cow * deltaTime;
        
        if (Collisions::CheckCowMovementAndPushBoxes(newPos, leftVec, boxPositions, boxSize)) {
            cowPosition = newPos;
        }
    }

    if (keyPressed[GLFW_KEY_D]) {
        glm::vec3 rightVec = glm::vec3(cowOrientation * glm::vec4(cowRight, 0.0f));
        glm::vec3 newPos = cowPosition + rightVec * speed_cow * deltaTime;
        
        if (Collisions::CheckCowMovementAndPushBoxes(newPos, rightVec, boxPositions, boxSize)) {
            cowPosition = newPos;
        }
    }

    if (keyPressed[GLFW_KEY_H]) {
        showInfoText = !showInfoText;
        keyPressed[GLFW_KEY_H] = false;
    }
}

void Game::updateSphereAnimation(float deltaTime) {
    sphereAnimationTime += deltaTime;
}

void Game::renderPermanentText() {
    TextRendering_PrintString(window.getNativeWindow(), "Pressione H para habilitar / desabilitar a ajuda.", -0.95f, 0.75f, 0.6f);
}

void Game::renderInfoText() {
    if (!showInfoText) {
        return;
    }

    TextRendering_PrintString(window.getNativeWindow(), "W,A,S,D para mover a vaca", -0.95f, 0.95f, 0.6f);
    TextRendering_PrintString(window.getNativeWindow(), "V para trocar a camera (free / look-at)", -0.95f, 0.90f, 0.6f);
    char buffer[23];
    snprintf(buffer, 50, "%ld/3 posicoes corretas", correctPlacementsCount); 
    TextRendering_PrintString(window.getNativeWindow(), buffer, -0.95f, 0.85f, 0.6f);

    TextRendering_PrintString(window.getNativeWindow(), "Objetivo: posicionar os cubos nas cores correspondentes", -0.95f, 0.80f, 0.6f);
}

void Game::renderWinMessage() {
    if (gameWon) {
        std::string winText = "VOCE VENCEU!";
        TextRendering_PrintString(window.getNativeWindow(), winText, 0.25f, 0.70f, 0.6f);
    }
}

void Game::renderFPS() {
    static float old_seconds = 0.0f;
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";

    ellapsed_frames += 1;
    float seconds = glfwGetTime();
    float ellapsed_seconds = seconds - old_seconds;

    if (ellapsed_seconds > 1.0f)
    {
        snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);
        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    if (showInfoText) {
        TextRendering_PrintString(window.getNativeWindow(), buffer, 0.75f, 0.95f, 0.6f);
    }
}

void Game::updateGameState() {
    if(gameWon) {
        return;
    }

    correctPlacementsCount = 0;
    float tile_size = 0.8f;

    for (size_t i = 0; i < boxPositions.size(); ++i) {
        Collisions::AABB box_aabb = Collisions::CreateBoxAABB(boxPositions[i], boxSize);
        Collisions::AABB tile_aabb = Collisions::CreateCeilingAABB(tilePositions[i], tile_size);

        if (Collisions::CheckBoxPlacement(box_aabb, tile_aabb)) {
            boxInPlace[i] = true;
            correctPlacementsCount++;
        } else {
            boxInPlace[i] = false;
        }
    }
    
    if (correctPlacementsCount == boxPositions.size()) {
        gameWon = true;
    }
}