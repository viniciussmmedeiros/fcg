#include "Camera.h"
#include <limits>
#include "utils.h"

Camera::Camera() { init(); }

void Camera::init() {
    freeX = cos(freeCameraPhi) * sin(freeCameraTheta);
    freeY = -sin(freeCameraPhi);
    freeZ = cos(freeCameraPhi) * cos(freeCameraTheta);
    camera_view_vector = glm::vec4(freeX, freeY, freeZ, 0.0f);
}

void Camera::cursorPosCallback(double dx, double dy) {
    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    // Atualizamos parâmetros da câmera com os deslocamentos
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
    // Computamos a posição da câmera utilizando coordenadas esféricas.  As
    // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
    // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
    // e ScrollCallback().
    
    // look-at, para visualizar o cenário / mapa
    if(!useFreeCamera) {
        float r = lookatCameraDistance;
        float y = r*sin(lookatCameraPhi);
        float z = r*cos(lookatCameraPhi)*cos(lookatCameraTheta);
        float x = r*cos(lookatCameraPhi)*sin(lookatCameraTheta);

        // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.    
        glm::vec4 camera_position_c  = glm::vec4(x,y,z,1.0f);
        glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f);
        glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c;
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);
        
        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        return Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);
    
    }
    
    // free camera --> posicionar como se fosse a visão da vaca
    freeX = cos(freeCameraPhi) * sin(freeCameraTheta);
    freeY = -sin(freeCameraPhi);
    freeZ = cos(freeCameraPhi) * cos(freeCameraTheta);
    camera_view_vector = glm::vec4(freeX, freeY, freeZ, 0.0f);
    
    return Matrix_Camera_View(freeCameraPosition, camera_view_vector, glm::vec4(0.0f,1.0f,0.0f,0.0f));
}

glm::mat4 Camera::getProjectionMatrix(float screenRatio) const {
    float nearplane = -0.1f; // Posição do "near plane"
    float farplane  = -10.0f; // Posição do "far plane"
    float field_of_view = 3.141592 / 3.0f;
    return Matrix_Perspective(field_of_view, screenRatio, nearplane, farplane);
}

bool Camera::getUseFreeCamera() {
    return useFreeCamera;
}