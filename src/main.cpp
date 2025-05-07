#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
#include "matrices.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);

float g_ScreenRatio = 1.0f;

int main()
{
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(800, 800, "Escape da Vaca", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{   
    glViewport(0, 0, width, height);
    g_ScreenRatio = (float)width / height;
}

void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    GLuint program_id = glCreateProgram();

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    glLinkProgram(program_id);

    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);
    
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";
        
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    return program_id;
}