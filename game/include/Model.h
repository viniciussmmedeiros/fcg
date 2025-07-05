#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "tiny_obj_loader.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "matrices.h"

using namespace std;

class Model {
    public:
        // FONTE: SceneObject baseado na struct 'SceneObject' dos laboratórios.
        struct SceneObject {
            std::string  name;              // Nome do objeto
            size_t first_index;             // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
            size_t num_indices;             // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
            GLenum rendering_mode;          // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
            GLuint vertex_array_object_id;  // ID do VAO onde estão armazenados os atributos do modelo
            glm::vec3 bbox_min;             // Axis-Aligned Bounding Box do objeto
            glm::vec3 bbox_max;
        };

        // construtor recebe o path para carregar o modelo.
        Model(const string& path);

        // desenha o objeto a partir do seu nome, definindo mínimo e máximo da bounding box
        void draw(const char* object_name, GLint g_bbox_min_uniform, GLint g_bbox_max);
    private:
        // FONTE: laboratórios.
        // mapeia 'sceneObjects' ao seu nome, para acesso rápido
        std::map<std::string, SceneObject> g_VirtualScene;
};