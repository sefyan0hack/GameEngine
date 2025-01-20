#include <core/Mesh.hpp>
#include <core/Global_H.hpp>
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

Mesh::Mesh(const std::vector<Vertex> &vertices, [[maybe_unused]] const std::vector<unsigned int> &indices): VAO(0), VBO(0), EBO(0)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
#ifdef USE_EBO
        if(vertices.size() != indices.size()){
            ERR("vert size != indces size");
        }
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
#endif
    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0) );
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)) );
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)) );

    vInSize = vertices.size();
}

// auto Mesh::setupMesh() -> void
// {
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
  
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);

//     glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);
// #ifdef USE_EBO
//         glGenBuffers(1, &EBO);
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
// #endif
//     // vertex positions
//     glEnableVertexAttribArray(0);	
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0) );
//     // vertex normals
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)) );
//     // vertex texture coords
//     glEnableVertexAttribArray(2);
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)) );

// }

auto Mesh::Draw(const Shader &shader, size_t InstanceCount) -> void
{
    shader.Use();
    glBindVertexArray(VAO);
    
    if(InstanceCount > 1)
#ifdef USE_EBO
        glDrawElementsInstanced(GL_TRIANGLES, vInSize, GL_UNSIGNED_INT, 0, InstanceCount);
#else
        glDrawArraysInstanced(GL_TRIANGLES, 0, vInSize, InstanceCount);
#endif

    else
    
#ifdef USE_EBO
        glDrawElements(GL_TRIANGLES, vInSize, GL_UNSIGNED_INT, 0);
#else
        glDrawArrays(GL_TRIANGLES, 0, vInSize);
#endif
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
}