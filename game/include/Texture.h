
#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "tiny_obj_loader.h"

using namespace std;

class Texture {
    private:
        GLuint textureId;
        GLuint samplerId;
        GLuint textureUnit;
    public:
        Texture();
        ~Texture();
        
        
        void LoadTextureImage(const char* filename, const GLuint numLoadedTextures); // Função que carrega imagens de textura
        
        GLuint getTextureId() const;
        GLuint getSamplerId() const;
        GLuint getTextureUnit() const;

        void bind() const;
        void unbind() const;
};