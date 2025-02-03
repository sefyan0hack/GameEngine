#include <core/Mesh.hpp>
#include <core/Log.hpp>
#include <type_traits>

Mesh::Mesh(const std::vector<Vertex> &vertices, [[maybe_unused]] const std::vector<GLuint> &indices): VAO(0), VBO(0), EBO(0)
{
    using VetexData = typename std::remove_cv_t<typename std::remove_reference_t<decltype(vertices)>::value_type>;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VetexData), vertices.data(), GL_STATIC_DRAW);
#ifdef USE_EBO
        if(vertices.size() != indices.size()){
            ERR("vert size != indces size");
        }
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
#endif
    GLuint index = 0;
    // vertex positions
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index++, decltype(VetexData::Position)::length(), GL_FLOAT, GL_FALSE, sizeof(VetexData), reinterpret_cast<void*>(offsetof(VetexData, Position)) );
    // vertex normals
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index++, decltype(VetexData::Normal)::length(), GL_FLOAT, GL_FALSE, sizeof(VetexData), reinterpret_cast<void*>(offsetof(VetexData, Normal)) );
    // vertex texture coords
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index++, decltype(VetexData::TexCoords)::length(), GL_FLOAT, GL_FALSE, sizeof(VetexData), reinterpret_cast<void*>(offsetof(VetexData, TexCoords)) );

    vInSize = static_cast<GLuint>(vertices.size());
    Log::Info("{}", *this);
}

Mesh::~Mesh()
{
}