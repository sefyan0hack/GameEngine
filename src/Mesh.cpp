#include "Mesh.hpp"
#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices): VAO(0), VBO(0), EBO(0)
{
    this->Vertices = vertices;
    this->Indices = indices;
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0) );
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)) );
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)) );

}

void Mesh::Draw(const Shader &shader, size_t InstanceCount) 
{
    shader.Use();
    glBindVertexArray(VAO);
    if(InstanceCount > 1)
        glDrawElementsInstanced(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0, InstanceCount);
    else
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
}