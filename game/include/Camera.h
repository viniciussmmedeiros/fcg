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
        glm::mat4 getProjectionMatrix(float screenRatio) const;

        void setPerspective(bool usePerspective) { 
            usePerspectiveProjection = usePerspective; 
        }

    private:
        glm::vec4 m_position;
        float cameraTheta;
        float cameraPhi;
        float cameraDistance;
        bool usePerspectiveProjection;
};