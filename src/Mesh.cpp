#include "Mesh.hpp"
#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    this->Vertices = vertices;
    this->Indices = indices;
    // this->textures = textures;
    // setupMesh(InsPos);
}

void Mesh::setupMesh(std::vector<glm::vec3> InsPos)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    
    GLuint UBO;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_ARRAY_BUFFER, UBO);
    glBufferData(GL_ARRAY_BUFFER, InsPos.size() * sizeof(glm::vec3), InsPos.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);
    glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) 
{
    shader.Use();
    glBindVertexArray(VAO);
    // glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0, 500*500);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
}