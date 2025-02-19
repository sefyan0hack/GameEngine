#include <core/Mesh.hpp>
#include <core/Log.hpp>
#include <type_traits>

namespace {

    constinit static AttributeInfo position {
        .size = 3,//decltype(Mesh::VetexData::Position)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = reinterpret_cast<GLvoid*>(offsetof(Mesh::VetexData, Position)),
        .divisor = 0,
    };
    
    constinit static AttributeInfo normals {
        .size = 3,//decltype(Mesh::VetexData::Normal)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = reinterpret_cast<GLvoid*>(offsetof(Mesh::VetexData, Normal)),
        .divisor = 0,
    };

    constinit static AttributeInfo texCoords {
        .size = 2,//decltype(Mesh::VetexData::TexCoords)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = reinterpret_cast<GLvoid*>(offsetof(Mesh::VetexData, TexCoords)),
        .divisor = 0,
    };
}

Mesh::Mesh(const std::vector<Vertex> &vertices, [[maybe_unused]] const std::vector<GLuint> &indices, std::string Name)
    : vertices(vertices)
    , vInSize(vertices.size())
    , attribs({position, normals, texCoords})
    , name(Name)
    , VBO(0)
    , EBO(0)
    , VAO(0)
{
    glGenVertexArrays(1, &VAO);
    // Log::Expect(glIsVertexArray(VAO) == GL_TRUE, "{} not VertexArray", VAO);
    glGenBuffers(1, &VBO);
    
    Log::Expect(VAO != 0, "VAO is {}", VAO);

    Log::print("Create VAO {}", VAO);

    Updata();

#ifdef USE_EBO
        if(vertices.size() != indices.size()){
            ERR("vert size != indces size");
        }
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
#endif

    PrepareAttribs();
    Count++;
    Log::Info("{}", *this);
}


Mesh::~Mesh()
{
    Count--;
    if(glIsBuffer(EBO) == GL_TRUE) glDeleteBuffers(1, &EBO);
    if(glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
    if(glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);
    Log::print("Delete VAO {}", VAO);
}

Mesh::Mesh(const Mesh& other)
    : vertices(other.vertices)
    , vInSize(other.vInSize)
    , attribs(other.attribs)
    , name(other.name)
    , VBO(0)
    , EBO(0)
    , VAO(0)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    Log::Expect(VAO != 0, "VAO == 0");
    Log::Expect(VBO != 0, "VBO == 0");
    Log::print("Create VAO {}", VAO);

    Updata();

    PrepareAttribs();
}

auto Mesh::operator=(const Mesh& other) -> Mesh&
{
    if(*this != other){
        this->vertices = other.vertices;
        this->vInSize = other.vInSize;
        this->attribs = other.attribs;
        this->name = other.name;
        this->VBO = 0;
        this->EBO = 0;
        this->VAO = 0;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        Log::Expect(VAO != 0, "VAO == 0");
        Log::Expect(VBO != 0, "VBO == 0");
        Log::print("Create VAO {}", VAO);

        Updata();
        
        PrepareAttribs();
    }
    return *this;
}

Mesh::Mesh(Mesh&& other) noexcept
    : vertices(other.vertices)
    , vInSize(other.vInSize)
    , attribs(other.attribs)
    , name(other.name)
    , VBO(other.VBO)
    , EBO(other.EBO)
    , VAO(other.VAO)
{
    other.vertices.clear();
    other.vInSize = 0;
    other.attribs.clear();
    other.name.clear();
    other.VBO = 0;
    other.EBO = 0;
    other.VAO = 0;
}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh&
{
    if(*this != other){
        this->vertices = other.vertices;
        this->vInSize = other.vInSize;
        this->attribs = other.attribs;
        this->name = other.name;
        this->VBO = other.VBO;
        this->EBO = other.EBO;
        this->VAO = other.VAO;

        other.vertices.clear();
        other.vInSize = 0;
        other.attribs.clear();
        other.name.clear();
        other.VBO = 0;
        other.EBO = 0;
        other.VAO = 0;
    }
    return *this;
}

auto Mesh::operator==(const Mesh &other) const -> bool
{
    return this->VAO == other.VAO; // for now every vao is unique
}

// auto Mesh::CloneVAO(GLuint src) -> GLuint
// {
//     GLuint clone = 0;
//     glGenVertexArrays(1, &clone);
//     if(clone == 0) Log::Error("clone is 0");
    
//     GLint maxAttributes = 0;
//     glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);

//     if(attribs.size() >= maxAttributes) Log::Error("attribs reched max `{}`", maxAttributes);

//     // glBindVertexArray(src);

//     // std::vector<AttributeInfo> attributes(attribs);
//     // for (GLuint atrInd = 0; atrInd < attribs; ++atrInd)
//     // {
//     //     glGetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &attributes[atrInd].enabled);

//     //     if(attributes[atrInd].enabled){
//     //         glGetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attributes[atrInd].size);
//     //         glGetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&attributes[atrInd].type);
//     //         glGetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, (GLint*)&attributes[atrInd].normalized);
//     //         glGetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &attributes[atrInd].stride);
//     //         glGetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &attributes[atrInd].divisor);
//     //         glGetVertexAttribPointerv(atrInd, GL_VERTEX_ATTRIB_ARRAY_POINTER, &attributes[atrInd].offset);
//     //     }
//     // }

//     Log::Expect(glIsBuffer(this->VBO) == GL_TRUE, "{} is not valid buffer", this->VBO);

//     glBindVertexArray(clone);
//     glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
//     for (auto atrb : attribs)
//     {
//         setAttribute(atrb);
//     }
//     // glBindVertexArray(0);
//     // glBindBuffer(GL_ARRAY_BUFFER, 0);

//     return clone;
// }
auto Mesh::CloneBuffer(GLenum type, GLuint src) -> GLuint
{
    if (glIsBuffer(src) == GL_FALSE) Log::Error("{} not a buffer", src);

    GLuint clone = 0;
    glGenBuffers(1, &clone);
    if(clone == 0) Log::Error("VBO clone is 0");

    glBindBuffer(type, src);
    
    GLint bufferSize = 0, usage = 0;
    glGetBufferParameteriv(type, GL_BUFFER_SIZE, &bufferSize);
    glGetBufferParameteriv(type, GL_BUFFER_USAGE, &usage);
    
    if (bufferSize > 0) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, clone);
        glBufferData(GL_COPY_WRITE_BUFFER, bufferSize, nullptr, usage);
        glCopyBufferSubData(type, GL_COPY_WRITE_BUFFER, 0, 0, bufferSize);
    } else {
        glDeleteBuffers(1, &clone);
        Log::Error("VBO bufferSize is 0");
    }

    // glBindBuffer(type, 0);
    // glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    return clone;
}

auto Mesh::CloneVBO(GLuint src) -> GLuint
{
    return CloneBuffer(GL_ARRAY_BUFFER, src);
}
auto Mesh::CloneEBO(GLuint src) -> GLuint
{
    #ifdef USE_EBO
    return CloneBuffer(GL_ELEMENT_ARRAY_BUFFER, src);
    #else
    return 0;
    #endif
}
auto Mesh::setAttribute(GLuint index, AttributeInfo att) -> void
{
    Log::Expect(att.size > 0 && att.size <= 4, "position.size : 0<{}<4 wrong", att.size);

    Log::Info(
        "attribute {} : size: {}, type: {}, normalized: {}, stride: {}, offset: {}",
        index,
        att.size,
        att.type,
        att.normalized,
        att.stride,
        att.offset
    );

    glVertexAttribPointer(
        index,
        att.size,
        att.type,
        att.normalized,
        att.stride,
        att.offset
    );

    glVertexAttribDivisor(index, att.divisor);
}

auto Mesh::PrepareAttribs() ->void
{
    auto currentVAO = CurrentVAO();
    auto currentVBO = CurrentVBO();

    // Log::Expect(glIsVertexArray(VAO) && VAO != 0, "VAO is 0");

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLuint index = 0;
    for(const auto& attrib : attribs){
        setAttribute(index++, attrib);
    }
    glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
    glBindVertexArray(currentVAO);
}

auto Mesh::EnableAttribs() const -> void
{
    auto currentVAO = CurrentVAO();
    auto currentVBO = CurrentVBO();

    // Log::Expect(glIsVertexArray(VAO) && VAO != 0, "VAO is 0");

    if(currentVAO == VAO){
        for(GLuint i = 0; i < attribs.size(); i++){
            glEnableVertexAttribArray(i);
        }
    }else{   
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        for(GLuint i = 0; i < attribs.size(); i++){
            glEnableVertexAttribArray(i);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
        glBindVertexArray(currentVAO);
    }
}

auto Mesh::DisableAttribs() const -> void
{
    auto currentVAO = CurrentVAO();
    auto currentVBO = CurrentVBO();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    for(GLuint i = 0; i < attribs.size(); i++){
        glDisableVertexAttribArray(i);
    }
    glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
    glBindVertexArray(currentVAO);
}

auto Mesh::CurrentVAO() -> GLuint
{
    GLint currentVAO = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    return currentVAO;
}

auto Mesh::CurrentVBO() -> GLuint
{
    GLint currentVBO = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
    return currentVBO;
}

auto Mesh::Updata() -> void
{
    auto currentVBO = CurrentVBO();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Mesh::VetexData), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
}