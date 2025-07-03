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

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
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

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0.0,1.0,0.5,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2 * n * dot(n,l);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    if ( object_id == SPHERE )
    {
        // Projeção esférica para a esfera
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec3 pos = position_model.xyz - bbox_center.xyz;
        float r = length(pos);
        
        float theta = atan(pos.x, pos.z);
        float phi = asin(pos.y / r);

        U = (theta + M_PI) / (2.0 * M_PI);
        V = (phi + M_PI_2) / M_PI;

        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.1,0.1,0.1);
        q = 20.0;
    }
    else if ( object_id == BUNNY || object_id == COW )
    {
        // Projeção planar para bunny/cow
        float minx = bbox_min.x;
        float maxx = bbox_max.x;
        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);

        Ks = vec3(0.2,0.2,0.2);
        Ka = vec3(0.05,0.05,0.05);
        q = 10.0;
    }
    else if ( 
        object_id == PLANE || object_id == FLOOR || object_id == WALL || object_id == CEILING || object_id == BOX )
    {
        // Coordenadas de textura do arquivo OBJ
        U = texcoords.x;
        V = texcoords.y;

        Ks = vec3(0.1,0.1,0.1);
        Ka = vec3(0.02,0.02,0.02);
        q = 5.0;
    }

    // Obtemos a refletância difusa a partir da leitura da imagem de textura
    vec3 Kd = texture(TextureImage0, vec2(U,V)).rgb;

    // Espectro da fonte de iluminação - aumentando a intensidade
    vec3 I = vec3(1.2,1.2,1.2);

    // Espectro da luz ambiente - aumentando significativamente
    vec3 Ia = vec3(0.4,0.4,0.4);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0,dot(n,l));

    // Termo ambiente - mais forte para iluminar áreas escuras
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term = Ks * I * pow(max(0, dot(r,v)), q);

    // Adicionar um termo de iluminação mínima para evitar superfícies totalmente pretas
    vec3 minimum_lighting = Kd * 0.2;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, ambiente e iluminação mínima
    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term + minimum_lighting;

    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}

