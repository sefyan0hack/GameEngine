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
    if(glIsBuffer(EBO) == GL_TRUE) glDeleteBuffers(1, &EBO);
    if(glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
    if(glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);
}
Mesh::Mesh(const Mesh& other)
    : VAO(0), VBO(0), EBO(0), vInSize(other.vInSize), attribs(other.attribs), name(other.name)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    for (GLuint attributeIndex = 0; attribs < 2; ++attributeIndex)
    {
        GLint size = 0;
        GLenum type = 0;
        GLboolean normalized = GL_FALSE;
        GLint stride = 0;
        void* pointer = nullptr;

        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_TYPE, reinterpret_cast<GLint*>(&type));
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, reinterpret_cast<GLint*>(&normalized));
        glGetVertexAttribPointerv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);

        glVertexAttribPointer(attributeIndex, size, type, normalized, stride, pointer);

    }

    GLint bufferSize = 0;

    #ifdef USE_EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
    #endif

    bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

    // source buffer to GL_COPY_READ_BUFFER
    glBindBuffer(GL_COPY_READ_BUFFER, other.VBO);

    // destination buffer to GL_COPY_WRITE_BUFFER
    glBindBuffer(GL_COPY_WRITE_BUFFER, this->VBO);

    // Copy the data from the source to the destination
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, bufferSize);

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

// auto Mesh::operator=(const Mesh& other) -> Mesh&
// {
//     if(*this != other){
//         return 
//     }
//     return *this;
// }

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), vInSize(other.vInSize), attribs(other.attribs), name(other.name)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.attribs = 0;
    other.name.clear();
}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh&
{
    if(*this != other){
        this->VAO = other.VAO;
        this->VBO = other.VBO;
        this->EBO = other.EBO;
        this->vInSize = other.vInSize;
        this->attribs = other.attribs;
        this->name = other.name;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.vInSize = 0;
        other.attribs = 0;
        other.name.clear();
    }
    return *this;
}

auto Mesh::operator==(const Mesh &other) const -> bool
{
    return this->VAO == other.VAO
        && this->VBO == other.VBO
        && this->EBO == other.EBO
        && this->vInSize == other.vInSize
        && this->attribs == other.attribs
        && this->name == other.name;
}