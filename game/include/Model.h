#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "tiny_obj_loader.h"

using namespace std;

class Model {
    public:
        Model(const string& path);

        void draw();

    private:
        struct SceneObject {
            size_t num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
            GLenum rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
            GLuint vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
        };
        vector<SceneObject> sceneObjects;
        vector<GLuint> buffers;
};