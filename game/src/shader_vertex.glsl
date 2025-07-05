#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int object_id;

// Identificadores para diferentes tipos de objeto
#define SPHERE 0
#define PLANE  2
#define COW    3
#define FLOOR 4
#define WALL 5
#define CEILING 6
#define BOX 7

// Atributos de vértice que serão enviados para o fragment shader ("out")
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;
out vec3 gouraud_color; // Cor calculada para iluminação Gouraud

void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja {+NDC2+}.
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slides 123-151 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    // Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
    texcoords = texture_coefficients;

    // CÁLCULO DA ILUMINAÇÃO GOURAUD PARA PAREDES/PLANOS
    if (object_id == PLANE || object_id == FLOOR || object_id == WALL || object_id == CEILING || object_id == BOX) {
        // Cálculos de iluminação no vertex shader (Gouraud)
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;
        
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(vec4(0.0,1.0,0.5,0.0)); // Mesma luz do fragment shader
        vec4 v = normalize(camera_position - p);
        vec4 r = -l + 2 * n * dot(n,l);
        
        // Propriedades do material para paredes/planos
        vec3 Kd = vec3(1.0, 1.0, 1.0); // Será modulado pela textura no fragment shader
        vec3 Ks = vec3(0.1,0.1,0.1);
        vec3 Ka = vec3(0.02,0.02,0.02);
        float q = 5.0;
        
        vec3 I = vec3(1.2,1.2,1.2);
        vec3 Ia = vec3(0.4,0.4,0.4);
        
        // Calcular iluminação Gouraud
        vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l));
        vec3 ambient_term = Ka * Ia;
        vec3 phong_specular_term = Ks * I * pow(max(0, dot(r,v)), q);
        vec3 minimum_lighting = Kd * 0.2;
        
        gouraud_color = lambert_diffuse_term + ambient_term + phong_specular_term + minimum_lighting;
    } else {
        // Para objetos com iluminação Phong (esfera, cow, bunny), 
        // não calculamos iluminação aqui
        gouraud_color = vec3(1.0, 1.0, 1.0); // Valor neutro
    }
}

