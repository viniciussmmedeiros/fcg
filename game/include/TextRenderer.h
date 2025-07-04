#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void TextRendering_Init();
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
float TextRendering_LineHeight(GLFWwindow* window);