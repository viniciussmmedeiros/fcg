#include "Camera.h"
#include <limits>
#include "utils.h"

Camera::Camera(){}

void Camera::cursorPosCallback(double dx, double dy) {
    // FONTE: laboratórios. Ângulo phi e atualização dos parâmetros da câmera com os deslocamentos.
    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    // Atualizamos parâmetros da câmera livre ou look-at com os deslocamentos.
    if(!useFreeCamera) {
        lookatCameraTheta -= 0.01f*dx;
        lookatCameraPhi   += 0.01f*dy;
    
        if (lookatCameraPhi > phimax){
            lookatCameraPhi = phimax;
        }
        if (lookatCameraPhi < phimin){
            lookatCameraPhi = phimin;
        }
    } else {
        freeCameraTheta -= 0.01f*dx;
        freeCameraPhi   += 0.01f*dy;
    
        if (freeCameraPhi > phimax){
            freeCameraPhi = phimax;
        }
        if (freeCameraPhi < phimin){
            freeCameraPhi = phimin;
        }
    }
}

glm::mat4 Camera::getVirtualCamera() {
    // FONTE: laboratórios. Cálculo da posição da câmera utilizando as coordenadas esféricas.
    
    glm::vec4 cameraUpVector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);

    // câmera look-at, olhando para a origem.
    if(!useFreeCamera) {
        float r = lookatCameraDistance;
        float y = r*sin(lookatCameraPhi);
        float z = r*cos(lookatCameraPhi)*cos(lookatCameraTheta);
        float x = r*cos(lookatCameraPhi)*sin(lookatCameraTheta);

        // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        glm::vec4 cameraPosition  = glm::vec4(x,y,z,1.0f);
        glm::vec4 cameraLookAtL    = glm::vec4(0.0f,0.0f,0.0f,1.0f);
        glm::vec4 cameraViewVector = cameraLookAtL - cameraPosition;

        // Computamos a matriz "View" utilizando os parâmetros da câmera para definir o sistema de coordenadas da câmera.
        return Matrix_Camera_View(cameraPosition, cameraViewVector, cameraUpVector);
    }
    
    // câmera livre, seguindo a movimentação da vaca.
    freeX = cos(freeCameraPhi) * sin(freeCameraTheta);
    freeY = -sin(freeCameraPhi);
    freeZ = cos(freeCameraPhi) * cos(freeCameraTheta);
    cameraViewVector = glm::vec4(freeX, freeY, freeZ, 0.0f);
    
    return Matrix_Camera_View(freeCameraPosition, cameraViewVector, cameraUpVector);
}

glm::mat4 Camera::getProjectionMatrix(float screenRatio) {
    // FONTE: laboratórios. Cálculo da matriz de projeção.
    float nearPlane = -0.1f; // Posição do "near plane"
    float farPlane  = 10.0f; // Posição do "far plane"
    float fieldOfView = 3.141592 / 3.0f;
    return Matrix_Perspective(fieldOfView, screenRatio, nearPlane, farPlane);
}

bool Camera::getUseFreeCamera() {
    return useFreeCamera;
}