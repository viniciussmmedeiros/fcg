// #ifndef _MATRICES_H
// #define _MATRICES_H

#include <cstdio>
#include <cstdlib>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Esta função Matrix() auxilia na criação de matrizes usando a biblioteca GLM.
// Note que em OpenGL (e GLM) as matrizes são definidas como "column-major",
// onde os elementos da matriz são armazenadas percorrendo as COLUNAS da mesma.
// Por exemplo, seja
//
//       [a b c]
//   M = [d e f]
//       [g h i]
//
// uma matriz 3x3. Em memória, na representação "column-major" de OpenGL, essa
// matriz é representada pelo seguinte array:
//
//   M[] = {  a,d,g,    b,e,h,    c,f,i  };
//              ^         ^         ^
//              |         |         |
//           coluna 1  coluna 2  coluna 3
//
// Para conseguirmos definir matrizes através de suas LINHAS, a função Matrix()
// computa a transposta usando os elementos passados por parâmetros.
glm::mat4 Matrix(
    float m00, float m01, float m02, float m03, // LINHA 1
    float m10, float m11, float m12, float m13, // LINHA 2
    float m20, float m21, float m22, float m23, // LINHA 3
    float m30, float m31, float m32, float m33  // LINHA 4
);
glm::mat4 Matrix_Identity();
glm::mat4 Matrix_Translate(float tx, float ty, float tz);
glm::mat4 Matrix_Scale(float sx, float sy, float sz);
glm::mat4 Matrix_Rotate_X(float angle);
glm::mat4 Matrix_Rotate_Y(float angle);
glm::mat4 Matrix_Rotate_Z(float angle);
float norm(glm::vec4 v);
glm::mat4 Matrix_Rotate(float angle, glm::vec4 axis);
glm::vec4 crossproduct(glm::vec4 u, glm::vec4 v);
float dotproduct(glm::vec4 u, glm::vec4 v);
glm::mat4 Matrix_Camera_View(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector);
glm::mat4 Matrix_Orthographic(float l, float r, float b, float t, float n, float f);
glm::mat4 Matrix_Perspective(float field_of_view, float aspect, float n, float f);
void PrintMatrix(glm::mat4 M);
void PrintVector(glm::vec4 v);
void PrintMatrixVectorProduct(glm::mat4 M, glm::vec4 v);
void PrintMatrixVectorProductDivW(glm::mat4 M, glm::vec4 v);