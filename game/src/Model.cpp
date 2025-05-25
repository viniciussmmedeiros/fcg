#include "Model.h"
#include <stdexcept>
#include <iostream>

using namespace std;

Model::Model(const string& path) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string warn, err;

    // bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw runtime_error(warn + err);
    }

    for (const auto& shape : shapes) {
        SceneObject sceneObject;
        sceneObject.rendering_mode = GL_TRIANGLES;
        
        vector<float> vertices;
        vector<float> normals;
        vector<GLuint> indices;

        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            vertices.push_back(1.0f);

            if (!attrib.normals.empty()) {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
                normals.push_back(0.0f);
            }
            indices.push_back(indices.size());
        }

        sceneObject.num_indices = indices.size();

        glGenVertexArrays(1, &sceneObject.vertex_array_object_id);
        glBindVertexArray(sceneObject.vertex_array_object_id);

        GLuint VBO_vertices, VBO_normals, EBO;
        glGenBuffers(1, &VBO_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        buffers.push_back(VBO_vertices);

        if (!normals.empty()) {
            glGenBuffers(1, &VBO_normals);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(1);
            buffers.push_back(VBO_normals);
        }
        
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        buffers.push_back(EBO);

        glBindVertexArray(0);
        sceneObjects.push_back(sceneObject);
    }
}

void Model::draw() {
    for (const auto& object : sceneObjects) {
        glBindVertexArray(object.vertex_array_object_id);
        glDrawElements(object.rendering_mode, object.num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}