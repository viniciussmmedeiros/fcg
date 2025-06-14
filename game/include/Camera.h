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

        void setPerspective(bool usePerspective) { 
            usePerspectiveProjection = usePerspective; 
        }

        void setFreeCamera(bool useFreeCameraMode) {
            useFreeCamera = useFreeCameraMode;
        }

        void setCameraPhi(float phi) {
            cameraPhi = phi;
        }

        void setCameraTheta(float theta) {
            cameraTheta = theta;
        }

        void setPosition(glm::vec3 position) {
            // printf("\nESTAMOS AQUI!!\n");
            m_position = glm::vec4(position, 1.0f);
            // printf("\t\tm_position: %f, %f, %f, %f\n", m_position.x, m_position.y, m_position.z, m_position.w);
        }

    private:
        glm::vec4 m_position;
        float cameraTheta;
        float cameraPhi;
        float cameraDistance;
        bool usePerspectiveProjection;
        bool useFreeCamera;
};