
#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "tiny_obj_loader.h"

using namespace std;

class Texture {
    private:
        // id da textura na gpu
        GLuint textureId;
        GLuint samplerId;
        // slot onde a textura está
        GLuint textureUnit;
    public:
        // método para carregar e configurar uma textura
        void LoadTextureImage(const char* filename, const GLuint numLoadedTextures); // Função que carrega imagens de textura
        
        // getter para buscar o slot da textura
        GLuint getTextureUnit();

        // métodos para ativar ou desativar a textura
        void bind();
        void unbind();
};