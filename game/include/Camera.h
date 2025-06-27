#pragma once // para evitar erros de 'redeclaração'
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "matrices.h"
#include <glad/glad.h>

class Camera {
    public:
        Camera();

        void cursorPosCallback(double dx, double dy);
        void scrollCallback(double yoffset);

        glm::mat4 getVirtualCamera();
        bool getUseFreeCamera();
        glm::mat4 getProjectionMatrix(float screenRatio) const;

        void setFreeCamera(bool useFreeCameraMode) {
            useFreeCamera = useFreeCameraMode;
        }

        void setFreeCameraPosition(glm::vec4 position) {
            freeCameraPosition = position;
        }

        glm::vec4 getFreeCameraPosition() {
            return freeCameraPosition;
        }

        glm::vec4 getCameraViewVector() {
            return camera_view_vector;
        }

    private:
        void init();
        
        float lookatCameraTheta = 0.0f;
        float lookatCameraPhi = 0.0f;
        float lookatCameraDistance = 2.5f;

        bool useFreeCamera = true;
        
        float freeCameraTheta = 4.141592f;
        float freeCameraPhi = 0.0f;
        float freeCameraDistance = 2.5f;
        glm::vec4 freeCameraPosition = glm::vec4(2.0f, 1.0f, 2.0f, 1.0f);

        float freeX = cos(freeCameraPhi) * sin(freeCameraTheta);
        float freeY = -sin(freeCameraPhi);
        float freeZ = cos(freeCameraPhi) * cos(freeCameraTheta);
        glm::vec4 camera_view_vector = glm::vec4(freeX, freeY, freeZ, 0.0f);
};