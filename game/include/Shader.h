#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setInt(const std::string& name, int value);

private:
    GLuint program_id;
    static std::string readFile(const std::string& path);
    static GLuint compileShader(const std::string& source, GLenum type);
};