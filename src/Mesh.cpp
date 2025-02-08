#include <core/Mesh.hpp>
#include <core/Log.hpp>
#include <type_traits>

Mesh::Mesh(const std::vector<Vertex> &vertices, [[maybe_unused]] const std::vector<GLuint> &indices, std::string Name)
    : VAO(0), VBO(0), EBO(0), vInSize(0), attribs(0), name(Name)
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
    glVertexAttribPointer(index++, decltype(VetexData::Position)::length(), GL_FLOAT, GL_FALSE, sizeof(VetexData), reinterpret_cast<void*>(offsetof(VetexData, Position)) );
    // vertex normals
    glVertexAttribPointer(index++, decltype(VetexData::Normal)::length(), GL_FLOAT, GL_FALSE, sizeof(VetexData), reinterpret_cast<void*>(offsetof(VetexData, Normal)) );
    // vertex texture coords
    glVertexAttribPointer(index++, decltype(VetexData::TexCoords)::length(), GL_FLOAT, GL_FALSE, sizeof(VetexData), reinterpret_cast<void*>(offsetof(VetexData, TexCoords)) );

    vInSize = static_cast<GLuint>(vertices.size());
    attribs = index;
    Count++;
    
    Log::Info("{}", *this);
}

Mesh::Mesh(const std::vector<GLfloat> vertices, std::string Name)
    : VAO(0), VBO(0), EBO(0), vInSize(0), attribs(0), name(Name)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    GLuint index = 0;
    // vertex positions
    glVertexAttribPointer(index++, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    // Normal attribute
    glVertexAttribPointer(index++, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    vInSize = static_cast<GLuint>(vertices.size()/3);
    attribs = index;
    Count++;
    Log::Info("{}", *this);
}

Mesh::~Mesh()
{
}

auto Mesh::EnableAttribs() const -> void
{
    for(GLuint i = 0; i < attribs; i++){
        glEnableVertexAttribArray(i);
    }
}
auto Mesh::DisableAttribs() const -> void
{
    for(GLuint i = 0; i < attribs; i++){
        glDisableVertexAttribArray(i);
    }
}