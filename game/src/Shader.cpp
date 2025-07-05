#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

Shader::Shader(const string& vertexPath, const string& fragmentPath) {
    // leitura dos arquivos .glsl
    string vertexCode = readFile(vertexPath);
    string fragmentCode = readFile(fragmentPath);

    // envia os arquivos glsl lidos para a gpu
    GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    // FONTE: laboratórios. Trecho equivalente à função 'CreateGpuProgram'.
    // criamos um programa na gpu e linkamos os glsl lidos e 'compilados' neles  
    program_id = glCreateProgram();
    glAttachShader(program_id, vertexShader);
    glAttachShader(program_id, fragmentShader);
    glLinkProgram(program_id);

    GLint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        vector<char> infoLog(log_length);
        glGetProgramInfoLog(program_id, log_length, NULL, infoLog.data());
        throw runtime_error("Shader Linking Failed: " + string(infoLog.data()));
    }

    // após linkarmos os shaders, os objetos lidos e 'compilados' podem ser descartados
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    // salvamos as localizações de bbox para uso posterior
    g_bbox_min_uniform = glGetUniformLocation(program_id, "bbox_min");
    g_bbox_max_uniform = glGetUniformLocation(program_id, "bbox_max");

    glUseProgram(program_id);
}

GLint Shader::getBBoxMinUniform() {
    return g_bbox_min_uniform;
}

GLint Shader::getBBoxMaxUniform() {
    return g_bbox_max_uniform;
}

// para as funções de setters basicamente buscamos a localização
// de uma variável dentro do shader pelo nome, e usamos glUniform
// para enviar o dado do argumento (mar, value, vec...) para
// a localização desejada na gpu.
void Shader::setMat4(const string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const string& name, int value) {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

void Shader::setVec4(const string& name, const glm::vec4& vec) {
    glUniform4fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) {
    glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, glm::value_ptr(vec));
}

string Shader::readFile(const string& path) {
    // FONTE: laboratório. Trecho equivalente ao início da função 'LoadShader'.
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Failed to open shader file: " + path);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(const string& source, GLenum type) {
    // FONTE: laboratório. Trecho equivalente à função 'LoadShader' e 'LoadShader_Fragment'.
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    string  output;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* log = new GLchar[log_length];
        glGetShaderInfoLog(shader, log_length, &log_length, log);

        output += "ERROR: OpenGL compilation of \"";
        output += source;
        output += "\" failed.\n";
        output += "== Start of compilation log\n";
        output += log;
        output += "== End of compilation log\n";
        fprintf(stderr, "%s", output.c_str());
    }

    return shader;
}