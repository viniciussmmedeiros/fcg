#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Atributos de iluminação interpolados do vertex shader (para Gouraud)
in vec3 gouraud_color;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define FRONT_BACK_WALL  1
#define PLANE  2
#define COW    3
#define FLOOR 4
#define WALL 5
#define CEILING 6
#define BOX 7
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

uniform vec3 object_game_color;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

// FONTE: BASEADO NO LABORATÓRIO 4 E 5

void main()
{
    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    if ( object_id == SPHERE )
    {
        // ILUMINAÇÃO PHONG PARA A ESFERA
        
        // Projeção esférica para a esfera
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec3 pos = position_model.xyz - bbox_center.xyz;
        float r = length(pos);
        
        float theta = atan(pos.x, pos.z);
        float phi = asin(pos.y / r);

        U = (theta + M_PI) / (2.0 * M_PI);
        V = (phi + M_PI_2) / M_PI;

        // Cálculos de iluminação Phong no fragment shader
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(vec4(0.0,1.0,0.5,0.0));
        vec4 v = normalize(camera_position - p);
        vec4 h = normalize(l + v);

        // Propriedades da esfera
        vec3 Kd = texture(TextureImage0, vec2(U,V)).rgb;
        vec3 Ks = vec3(0.3,0.3,0.3);
        vec3 Ka = vec3(0.1,0.1,0.1);
        float q = 20.0;

        vec3 I = vec3(1.2,1.2,1.2);
        vec3 Ia = vec3(0.4,0.4,0.4);

        vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l));
        vec3 ambient_term = Ka * Ia;
        vec3 blinn_phong_specular_term = Ks * I * pow(max(0, dot(n,h)), q);
        vec3 minimum_lighting = Kd * 0.2;

        color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term + minimum_lighting * object_game_color;
    }
    else if ( object_id == FRONT_BACK_WALL || object_id == COW )
    {
        // ILUMINAÇÃO PHONG PARA FRONT_BACK_WALL/COW
        
        // Projeção planar para FRONT_BACK_WALL/cow
        float minx = bbox_min.x;
        float maxx = bbox_max.x;
        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);

        // Cálculos de iluminação Phong no fragment shader
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(vec4(0.0,1.0,0.5,0.0));
        vec4 v = normalize(camera_position - p);
        vec4 h = normalize(l + v);

        vec3 Kd = texture(TextureImage0, vec2(U,V)).rgb;
        vec3 Ks = vec3(0.2,0.2,0.2);
        vec3 Ka = vec3(0.05,0.05,0.05);
        float q = 10.0;

        vec3 I = vec3(1.2,1.2,1.2);
        vec3 Ia = vec3(0.4,0.4,0.4);

        vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l));
        vec3 ambient_term = Ka * Ia;
        // FONTE: slide 150, modelos de iluminação
        vec3 blinn_phong_specular_term = Ks * I * pow(max(0, dot(n,h)), q);
        vec3 minimum_lighting = Kd * 0.2;

        color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term + minimum_lighting;
    }
    else if ( 
        object_id == PLANE || object_id == FLOOR || object_id == WALL || object_id == CEILING || object_id == BOX )
    {        
        // Coordenadas de textura do arquivo OBJ
        U = texcoords.x;
        V = texcoords.y;

        // Obtém a cor da textura
        vec3 texture_color = texture(TextureImage0, vec2(U,V)).rgb;
        
        // Usa a cor interpolada do vertex shader (Gouraud) modulada pela textura
        color.rgb = gouraud_color * texture_color * object_game_color;
    }

    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}

