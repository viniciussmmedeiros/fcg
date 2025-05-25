#include "Camera.h"
#include <limits>
#include "utils.h"

Camera::Camera() : cameraTheta(0.0f), cameraPhi(0.0f), cameraDistance(3.5f), usePerspectiveProjection(true) {}

void Camera::cursorPosCallback(double dx, double dy) {
    // Atualizamos parâmetros da câmera com os deslocamentos
    cameraTheta -= 0.01f*dx;
    cameraPhi   += 0.01f*dy;

    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (cameraPhi > phimax){
        cameraPhi = phimax;
    }
    if (cameraPhi < phimin){
        cameraPhi = phimin;
    }
}

void Camera::scrollCallback(double yoffset) {
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    cameraDistance -= 0.1f * yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (cameraDistance < verysmallnumber) {
        cameraDistance = verysmallnumber;
    }
}

glm::mat4 Camera::getVirtualCamera() {
    // Computamos a posição da câmera utilizando coordenadas esféricas.  As
    // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
    // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
    // e ScrollCallback().
    float r = cameraDistance;
    float y = r * sin(cameraPhi);
    float z = r * cos(cameraPhi) * cos(cameraTheta);
    float x = r * cos(cameraPhi) * sin(cameraTheta);
    
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

glm::mat4 Camera::getProjectionMatrix(float screenRatio) const {
    float nearplane = -0.1f; // Posição do "near plane"
    float farplane  = -10.0f; // Posição do "far plane"

    if (usePerspectiveProjection) {
        // Projeção Perspectiva.
        // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.    
    
        float field_of_view = 3.141592 / 3.0f;
        return Matrix_Perspective(field_of_view, screenRatio, nearplane, farplane);
    } else {
        // Projeção Ortográfica.
        // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
        // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
        // Para simular um "zoom" ortográfico, computamos o valor de "t"
        // utilizando a variável g_CameraDistance.
        float t = 1.5f * cameraDistance / 2.5f;
        float b = -t;
        float r = t * screenRatio;
        float l = -r;
        return Matrix_Orthographic(l, r, b, t, nearplane, farplane);
    }
}