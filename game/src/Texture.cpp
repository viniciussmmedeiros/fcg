#include "Texture.h"
#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>

Texture::Texture() : textureId(0), samplerId(0) { }

Texture::~Texture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
    }
    if (samplerId != 0) {
        glDeleteSamplers(1, &samplerId);
    }
}

void Texture::LoadTextureImage(const char* filename, const GLuint numLoadedTextures) {
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    this->textureUnit = numLoadedTextures;

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    // GLuint texture_id;
    // GLuint sampler_id;
    // glGenTextures(1, &texture_id);
    // glGenSamplers(1, &sampler_id);
    glGenTextures(1, &this->textureId);
    glGenSamplers(1, &this->samplerId);
    // glGenTextures(1, &textureId);
    // glGenSamplers(1, &samplerId);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    // glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glSamplerParameteri(this->samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(this->samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    // glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(this->samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(this->samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    // GLuint textureunit = numLoadedTextures;
    // glActiveTexture(GL_TEXTURE0 + textureunit);
    // glBindTexture(GL_TEXTURE_2D, texture_id);
    // // glActiveTexture(GL_TEXTURE0 + textureUnit);
    // // glBindTexture(GL_TEXTURE_2D, textureId);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // // glBindSampler(textureUnit, samplerId);
    // glBindSampler(textureunit, sampler_id);

    glActiveTexture(GL_TEXTURE0 + this->textureUnit);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindSampler(this->textureUnit, this->samplerId);
    
    stbi_image_free(data);
}

GLuint Texture::getTextureId() const {
    return textureId;
}

GLuint Texture::getSamplerId() const {
    return samplerId;
}

GLuint Texture::getTextureUnit() const {
    return textureUnit;
}

void Texture::bind() const {
    printf("texture id : %d\n", textureId);
    printf("texture unit : %d\n", textureUnit);
    // if (textureId != 0) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindSampler(textureUnit, samplerId);
    // }
}

void Texture::unbind() const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindSampler(textureUnit, 0);
}