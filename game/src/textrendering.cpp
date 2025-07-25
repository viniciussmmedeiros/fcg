// Based on http://hamelot.io/visualization/opengl-text-without-any-external-libraries/
//   and on https://github.com/rougier/freetype-gl
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "dejavufont.h"

// FONTE: textrendering.cpp do laboratório.

static GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
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
const GLchar* const textvertexshader_source = ""
"#version 330\n"
"layout (location = 0) in vec4 position;\n"
"out vec2 texCoords;\n"
"void main()\n"
"{\n"
    "gl_Position = vec4(position.xy, 0, 1);\n"
    "texCoords = position.zw;\n"
"}\n"
"\0";

const GLchar* const textfragmentshader_source = ""
"#version 330\n"
"uniform sampler2D tex;\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
    "fragColor = vec4(102.0, 204.0, 0, texture(tex, texCoords).r);\n"
"}\n"
"\0";

void TextRendering_LoadShader(const GLchar* const shader_string, GLuint shader_id)
{
    // Define o código do shader, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, NULL);

    // Compila o código do shader (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "ERROR: OpenGL compilation failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

GLuint textVAO;
GLuint textVBO;
GLuint textprogram_id;
GLuint texttexture_id;

void TextRendering_Init()
{
    GLuint sampler;

    glGenBuffers(1, &textVBO);
    glGenVertexArrays(1, &textVAO);
    glGenTextures(1, &texttexture_id);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCheckError();

    GLuint textvertexshader_id = glCreateShader(GL_VERTEX_SHADER);
    TextRendering_LoadShader(textvertexshader_source, textvertexshader_id);
    glCheckError();

    GLuint textfragmentshader_id = glCreateShader(GL_FRAGMENT_SHADER);
    TextRendering_LoadShader(textfragmentshader_source, textfragmentshader_id);
    glCheckError();

    textprogram_id = CreateGpuProgram(textvertexshader_id, textfragmentshader_id);
    glLinkProgram(textprogram_id);
    glCheckError();

    GLuint texttex_uniform;
    texttex_uniform = glGetUniformLocation(textprogram_id, "tex");
    glCheckError();

    GLuint textureunit = 31;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texttexture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, dejavufont.tex_width, dejavufont.tex_height, 0, GL_RED, GL_UNSIGNED_BYTE, dejavufont.tex_data);
    glBindSampler(textureunit, sampler);
    glCheckError();

    glBindVertexArray(textVAO);

    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glCheckError();

    glUseProgram(textprogram_id);
    glUniform1i(texttex_uniform, textureunit);
    glUseProgram(0);
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glCheckError();
}

float textscale = 3.0f;

void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f)
{
    scale *= textscale;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float sx = scale / width;
    float sy = scale / height;

    for (size_t i = 0; i < str.size(); i++)
    {
        // Find the glyph for the character we are looking for
        texture_glyph_t *glyph = 0;
        for (size_t j = 0; j < dejavufont.glyphs_count; ++j)
        {
            if (dejavufont.glyphs[j].codepoint == (uint32_t)str[i])
            {
                glyph = &dejavufont.glyphs[j];
                break;
            }
        }
        if (!glyph) {
            continue;
        }
        x += glyph->kerning[0].kerning;
        float x0 = (float) (x + glyph->offset_x * sx);
        float y0 = (float) (y + glyph->offset_y * sy);
        float x1 = (float) (x0 + glyph->width * sx);
        float y1 = (float) (y0 - glyph->height * sy);

        float s0 = glyph->s0 - 0.5f/dejavufont.tex_width;
        float t0 = glyph->t0 - 0.5f/dejavufont.tex_height;
        float s1 = glyph->s1 - 0.5f/dejavufont.tex_width;
        float t1 = glyph->t1 - 0.5f/dejavufont.tex_height;

        struct {float x, y, s, t;} data[6] = {
            { x0, y0, s0, t0 },
            { x0, y1, s0, t1 },
            { x1, y1, s1, t1 },
            { x0, y0, s0, t0 },
            { x1, y1, s1, t1 },
            { x1, y0, s1, t0 }
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDepthFunc(GL_ALWAYS);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(float), data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUseProgram(textprogram_id);
        glBindVertexArray(textVAO);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glUseProgram(0);
        glDepthFunc(GL_LESS);

        glDisable(GL_BLEND);

        x += (glyph->advance_x * sx);
    }
}

float TextRendering_LineHeight(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return dejavufont.height / height * textscale;
}

float TextRendering_CharWidth(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return dejavufont.glyphs[32].advance_x / width * textscale;
}
