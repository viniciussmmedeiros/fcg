#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        // método para ativar esse shader para uso, chamado antes de desenhar os objetos
        void use();

        // métodos para mandar dados 'uniform' para dentro do shader
        void setMat4(const std::string& name, const glm::mat4& mat);
        void setInt(const std::string& name, int value);
        void setVec3(const std::string& name, const glm::vec3& vec);
        void setVec4(const std::string& name, const glm::vec4& vec);

        GLint getBBoxMinUniform();
        GLint getBBoxMaxUniform();
    private:
        // identificador do nosso shader na gpu
        GLuint program_id;
        mutable GLint g_bbox_min_uniform;
        mutable GLint g_bbox_max_uniform;
        static std::string readFile(const std::string& path);
        static GLuint compileShader(const std::string& source, GLenum type);
};