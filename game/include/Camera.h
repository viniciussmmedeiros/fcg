#pragma once // para evitar erros de redeclaração
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "matrices.h"
#include <glad/glad.h>

class Camera {
    public:
        Camera(); // construtor default.

        // lida com o movimento do mouse para atualizar os ângulos da câmera.
        void cursorPosCallback(double dx, double dy);

        // retorna a matriz de visualização atual da câmera (em coordenadas da câmera).
        glm::mat4 getVirtualCamera();
        
        // retorna se a câmera está no modo 'livre' ou não.
        bool getUseFreeCamera();
        
        // retorna a matriz de projeção conforme a proporção da tela.
        glm::mat4 getProjectionMatrix(float screenRatio);

        // retorna o 'viewVector', usado para computar a câmera livre em relação a vaca.
        glm::vec4 getCameraViewVector() {
            return cameraViewVector;
        }

        // atualiza o modo da câmera para 'free' ou não quando a tecla 'V' é pressionada.
        void setUseFreeCamera(bool useFreeCameraMode) {
            useFreeCamera = useFreeCameraMode;
        }

        // retorna o ângulo theta da câmera, usado para rotacionar o modelo na vaca.
        float getFreeCameraTheta() {
            return freeCameraTheta;
        }

        // atualiza a posição da câmera livre conforme a posição da vaca.
        void setFreeCameraPosition(glm::vec4 position) {
            freeCameraPosition = position;
        }

    private:
        bool useFreeCamera = true; // iniciamos sempre em modo 'free camera'.
        
        // FONTE: valores usados no lab 2 para implementação das coordenadas esféricas da câmera.
        float lookatCameraTheta = 0.0f; // ângulo horizontal
        float lookatCameraPhi = 0.0f; // ângulo vertical
        float lookatCameraDistance = 2.5f; // distância da origem
        
        float freeCameraTheta = 4.141592f; // ângulo horizontal inicial
        float freeCameraPhi = 0.0f; // ângulo vertical inicial
        // valores do vetor de direção (view vector)
        float freeX = cos(freeCameraPhi) * sin(freeCameraTheta);
        float freeY = -sin(freeCameraPhi);
        float freeZ = cos(freeCameraPhi) * cos(freeCameraTheta);
        glm::vec4 freeCameraPosition = glm::vec4(2.0f, 1.0f, 2.0f, 1.0f);
        
        // vetor de direção da câmera, inicialmente é calculado para câmera livre
        glm::vec4 cameraViewVector = glm::vec4(freeX, freeY, freeZ, 0.0f);
};