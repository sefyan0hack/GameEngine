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
    // glBindVertexArray(0);
    
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

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    // Copy vertex data
    glBindBuffer(GL_COPY_READ_BUFFER, other.VBO);
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    
    if (bufferSize > 0) {
        glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, bufferSize);
    } else {
        Log::Error("VBO bufferSize is 0");
    }
    
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindVertexArray(other.VAO);

    std::vector<GLint> sizes(attribs);
    std::vector<GLenum> types(attribs);
    std::vector<GLboolean> normalized(attribs);
    std::vector<GLint> strides(attribs);
    std::vector<void*> pointers(attribs);
    for (GLuint attributeIndex = 0; attributeIndex < attribs; ++attributeIndex)
    {
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_SIZE, &sizes[attributeIndex]);
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_TYPE, reinterpret_cast<GLint*>(&types[attributeIndex]));
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, reinterpret_cast<GLint*>(&normalized[attributeIndex]));
        glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &strides[attributeIndex]);
        glGetVertexAttribPointerv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointers[attributeIndex]);
    }
    glBindVertexArray(this->VAO);

    bufferSize = 0;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    for (GLuint attributeIndex = 0; attributeIndex < attribs; ++attributeIndex)
    {
        glVertexAttribPointer(attributeIndex, sizes[attributeIndex], types[attributeIndex],
        normalized[attributeIndex], strides[attributeIndex], pointers[attributeIndex]);
        glEnableVertexAttribArray(attributeIndex);
    }

    #ifdef USE_EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

        glBindBuffer(GL_COPY_READ_BUFFER, other.EBO);
        bufferSize = 0;
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        
        if (bufferSize > 0) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ELEMENT_ARRAY_BUFFER, 0, 0, bufferSize);
        } else {
            Log::Error("EBO bufferSize is 0");
        }
        
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
    #endif
}

auto Mesh::operator=(const Mesh& other) -> Mesh&
{
    if(*this != other){
        this->VAO = 0;
        this->VBO = 0;
        this->EBO = 0;
        this->vInSize = other.vInSize;
        this->attribs = other.attribs;
        this->name = other.name;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    
        // Copy vertex data
        glBindBuffer(GL_COPY_READ_BUFFER, other.VBO);
        GLint bufferSize = 0;
        glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        
        if (bufferSize > 0) {
            glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, bufferSize);
        } else {
            Log::Error("VBO bufferSize is 0");
        }
        
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
        glBindVertexArray(other.VAO);
    
        std::vector<GLint> sizes(attribs);
        std::vector<GLenum> types(attribs);
        std::vector<GLboolean> normalized(attribs);
        std::vector<GLint> strides(attribs);
        std::vector<void*> pointers(attribs);
        for (GLuint attributeIndex = 0; attributeIndex < attribs; ++attributeIndex)
        {
            glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_SIZE, &sizes[attributeIndex]);
            glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_TYPE, reinterpret_cast<GLint*>(&types[attributeIndex]));
            glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, reinterpret_cast<GLint*>(&normalized[attributeIndex]));
            glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &strides[attributeIndex]);
            glGetVertexAttribPointerv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointers[attributeIndex]);
        }
        glBindVertexArray(this->VAO);
    
        bufferSize = 0;
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        for (GLuint attributeIndex = 0; attributeIndex < attribs; ++attributeIndex)
        {
            glVertexAttribPointer(attributeIndex, sizes[attributeIndex], types[attributeIndex],
            normalized[attributeIndex], strides[attributeIndex], pointers[attributeIndex]);
            glEnableVertexAttribArray(attributeIndex);
        }
    
        #ifdef USE_EBO
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    
            glBindBuffer(GL_COPY_READ_BUFFER, other.EBO);
            bufferSize = 0;
            glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &bufferSize);
            
            if (bufferSize > 0) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
                glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ELEMENT_ARRAY_BUFFER, 0, 0, bufferSize);
            } else {
                Log::Error("EBO bufferSize is 0");
            }
            
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
        #endif 
    }
    return *this;
}

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