#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

Shader::Shader(const string& vertexPath, const string& fragmentPath) {
    string vertexCode = readFile(vertexPath);
    string fragmentCode = readFile(fragmentPath);

    GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const {
    glUseProgram(program_id);
}

void Shader::setMat4(const string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const string& name, int value) {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

string Shader::readFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Failed to open shader file: " + path);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(const string& source, GLenum type) {
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