
#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "tiny_obj_loader.h"

using namespace std;

class Texture {
    public:
        void LoadTextureImage(const char* filename, const GLuint numLoadedTextures); // Função que carrega imagens de textura
        
};