#include <core/Mesh.hpp>
#include <core/Log.hpp>

namespace {

[[maybe_unused]] auto to_string(GLenum type) -> const char*
{
    switch (type)
    {
        case GL_FLOAT: return "GL_FLOAT";
        case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
        case GL_DOUBLE: return "GL_DOUBLE";
        case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
        case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
        case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
        case GL_INT: return "GL_INT";
        case GL_INT_VEC2: return "GL_INT_VEC2";
        case GL_INT_VEC3: return "GL_INT_VEC3";
        case GL_INT_VEC4: return "GL_INT_VEC4";
        case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
        case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
        case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
        case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
        case GL_BOOL: return "GL_BOOL";
        case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
        case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
        case GL_BOOL_VEC4: return "GL_BOOL_VEC4";
        case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
        case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
        case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
        case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
        case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
        case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
        case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
        case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
        case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
        case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
        case GL_DOUBLE_MAT2x3: return "GL_DOUBLE_MAT2x3";
        case GL_DOUBLE_MAT2x4: return "GL_DOUBLE_MAT2x4";
        case GL_DOUBLE_MAT3x2: return "GL_DOUBLE_MAT3x2";
        case GL_DOUBLE_MAT3x4: return "GL_DOUBLE_MAT3x4";
        case GL_DOUBLE_MAT4x2: return "GL_DOUBLE_MAT4x2";
        case GL_DOUBLE_MAT4x3: return "GL_DOUBLE_MAT4x3";
        case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
        case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
        case GL_SAMPLER_1D_SHADOW: return "GL_SAMPLER_1D_SHADOW";
        case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
        case GL_SAMPLER_1D_ARRAY: return "GL_SAMPLER_1D_ARRAY";
        case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
        case GL_SAMPLER_1D_ARRAY_SHADOW: return "GL_SAMPLER_1D_ARRAY_SHADOW";
        case GL_SAMPLER_2D_ARRAY_SHADOW: return "GL_SAMPLER_2D_ARRAY_SHADOW";
        case GL_SAMPLER_2D_MULTISAMPLE: return "GL_SAMPLER_2D_MULTISAMPLE";
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";
        case GL_SAMPLER_BUFFER: return "GL_SAMPLER_BUFFER";
        case GL_SAMPLER_2D_RECT: return "GL_SAMPLER_2D_RECT";
        case GL_SAMPLER_2D_RECT_SHADOW: return "GL_SAMPLER_2D_RECT_SHADOW";
        case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
        case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
        case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
        case GL_INT_SAMPLER_CUBE: return "GL_INT_SAMPLER_CUBE";
        case GL_INT_SAMPLER_1D_ARRAY: return "GL_INT_SAMPLER_1D_ARRAY";
        case GL_INT_SAMPLER_2D_ARRAY: return "GL_INT_SAMPLER_2D_ARRAY";
        case GL_INT_SAMPLER_2D_MULTISAMPLE: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_INT_SAMPLER_BUFFER: return "GL_INT_SAMPLER_BUFFER";
        case GL_INT_SAMPLER_2D_RECT: return "GL_INT_SAMPLER_2D_RECT";
        case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
        case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
        case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
        case GL_UNSIGNED_INT_SAMPLER_CUBE: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
        case GL_IMAGE_1D: return "GL_IMAGE_1D";
        case GL_IMAGE_2D: return "GL_IMAGE_2D";
        case GL_IMAGE_3D: return "GL_IMAGE_3D";
        case GL_IMAGE_2D_RECT: return "GL_IMAGE_2D_RECT";
        case GL_IMAGE_CUBE: return "GL_IMAGE_CUBE";
        case GL_IMAGE_BUFFER: return "GL_IMAGE_BUFFER";
        case GL_IMAGE_1D_ARRAY: return "GL_IMAGE_1D_ARRAY";
        case GL_IMAGE_2D_ARRAY: return "GL_IMAGE_2D_ARRAY";
        case GL_IMAGE_2D_MULTISAMPLE: return "GL_IMAGE_2D_MULTISAMPLE";
        case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_IMAGE_2D_MULTISAMPLE_ARRAY";
        case GL_INT_IMAGE_1D: return "GL_INT_IMAGE_1D";
        case GL_INT_IMAGE_2D: return "GL_INT_IMAGE_2D";
        case GL_INT_IMAGE_3D: return "GL_INT_IMAGE_3D";
        case GL_INT_IMAGE_2D_RECT: return "GL_INT_IMAGE_2D_RECT";
        case GL_INT_IMAGE_CUBE: return "GL_INT_IMAGE_CUBE";
        case GL_INT_IMAGE_BUFFER: return "GL_INT_IMAGE_BUFFER";
        case GL_INT_IMAGE_1D_ARRAY: return "GL_INT_IMAGE_1D_ARRAY";
        case GL_INT_IMAGE_2D_ARRAY: return "GL_INT_IMAGE_2D_ARRAY";
        case GL_INT_IMAGE_2D_MULTISAMPLE: return "GL_INT_IMAGE_2D_MULTISAMPLE";
        case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_1D: return "GL_UNSIGNED_INT_IMAGE_1D";
        case GL_UNSIGNED_INT_IMAGE_2D: return "GL_UNSIGNED_INT_IMAGE_2D";
        case GL_UNSIGNED_INT_IMAGE_3D: return "GL_UNSIGNED_INT_IMAGE_3D";
        case GL_UNSIGNED_INT_IMAGE_2D_RECT: return "GL_UNSIGNED_INT_IMAGE_2D_RECT";
        case GL_UNSIGNED_INT_IMAGE_CUBE: return "GL_UNSIGNED_INT_IMAGE_CUBE";
        case GL_UNSIGNED_INT_IMAGE_BUFFER: return "GL_UNSIGNED_INT_IMAGE_BUFFER";
        case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return "GL_UNSIGNED_INT_IMAGE_1D_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return "GL_UNSIGNED_INT_IMAGE_2D_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE";
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_ATOMIC_COUNTER: return "GL_UNSIGNED_INT_ATOMIC_COUNTER";
        default: return "unknown";
    }
}
    constexpr static AttributeInfo position {
        .size = 3,//decltype(Mesh::VetexData::Position)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsetof(Mesh::VetexData, Position))),
        .divisor = 0,
    };
    
    constexpr static AttributeInfo normals {
        .size = 3,//decltype(Mesh::VetexData::Normal)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsetof(Mesh::VetexData, Normal))),
        .divisor = 0,
    };

    constexpr static AttributeInfo texCoords {
        .size = 2,//decltype(Mesh::VetexData::TexCoords)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsetof(Mesh::VetexData, TexCoords))),
        .divisor = 0,
    };
}

Mesh::Mesh(const std::vector<Vertex> &vertices, [[maybe_unused]] const std::vector<GLuint> &indices, std::string Name)
    : name(Name)
    , vertices(vertices)
    , attribs({position, normals, texCoords})
    , VBO(GenBuffer())
    , EBO(0)
    , VAO(GenVertexArray())
{
#ifdef USE_EBO
        if(vertices.size() != indices.size()){
            ERR("vert size != indces size");
        }
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
#endif

    PrepareVertexdata();
    PrepareAttribs();
    Count++;
    Info("{}", *this);
}


Mesh::~Mesh()
{
    Count--;
    if(glIsBuffer(EBO) == GL_TRUE) glDeleteBuffers(1, &EBO);
    if(glIsBuffer(VBO) == GL_TRUE) glDeleteBuffers(1, &VBO);
    if(glIsVertexArray(VAO) == GL_TRUE) glDeleteVertexArrays(1, &VAO);
}

Mesh::Mesh(const Mesh& other)
    : name(other.name)
    , vertices(other.vertices)
    , attribs(other.attribs)
    , VBO(GenBuffer())
    , EBO(0)
    , VAO(GenVertexArray())
{

    PrepareVertexdata();
    PrepareAttribs();
}

auto Mesh::operator=(const Mesh& other) -> Mesh&
{
    if(*this != other){
        this->name = other.name;
        this->vertices = other.vertices;
        this->attribs = other.attribs;
        this->VBO = GenBuffer();
        this->EBO = 0;
        this->VAO = GenVertexArray();
        
        PrepareVertexdata();
        PrepareAttribs();
    }
    return *this;
}

Mesh::Mesh(Mesh&& other) noexcept
    : name(other.name)
    , vertices(other.vertices)
    , attribs(other.attribs)
    , VBO(other.VBO)
    , EBO(other.EBO)
    , VAO(other.VAO)
{
    other.name.clear();
    other.vertices.clear();
    other.attribs.clear();
    other.VBO = 0;
    other.EBO = 0;
    other.VAO = 0;
}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh&
{
    if(*this != other){
        this->name = other.name;
        this->vertices = other.vertices;
        this->attribs = other.attribs;
        this->VBO = other.VBO;
        this->EBO = other.EBO;
        this->VAO = other.VAO;

        other.name.clear();
        other.vertices.clear();
        other.attribs.clear();
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
//     if(clone == 0) Error("clone is 0");
    
//     GLint maxAttributes = 0;
//     glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);

//     if(attribs.size() >= maxAttributes) Error("attribs reched max `{}`", maxAttributes);

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

//     Expect(glIsBuffer(this->VBO) == GL_TRUE, "{} is not valid buffer", this->VBO);

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
    GLuint clone = GenBuffer();
    if(clone == 0) Error("VBO clone is 0");

    BindBuffer(type, src);
    
    GLint bufferSize = 0, usage = 0;
    glGetBufferParameteriv(type, GL_BUFFER_SIZE, &bufferSize);
    glGetBufferParameteriv(type, GL_BUFFER_USAGE, &usage);
    
    if (bufferSize > 0) {
        BindBuffer(GL_COPY_WRITE_BUFFER, clone);
        glBufferData(GL_COPY_WRITE_BUFFER, bufferSize, nullptr, usage);
        glCopyBufferSubData(type, GL_COPY_WRITE_BUFFER, 0, 0, bufferSize);
    } else {
        glDeleteBuffers(1, &clone);
        Error("VBO bufferSize is 0");
    }

    return clone;
}

auto Mesh::CloneVBO(GLuint src) -> GLuint
{
    return CloneBuffer(GL_ARRAY_BUFFER, src);
}

auto Mesh::CloneEBO([[maybe_unused]] GLuint src) -> GLuint
{
    #ifdef USE_EBO
    return CloneBuffer(GL_ELEMENT_ARRAY_BUFFER, src);
    #else
    return 0;
    #endif
}
auto Mesh::setAttribute(GLuint index, AttributeInfo att) -> void
{
    Expect(att.size > 0 && att.size <= 4, "position.size : 0<{}<4 wrong", att.size);

    Info(
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

    BindVertexArray(VAO);
    BindVertexBuffer(VBO);
    GLuint index = 0;
    for(const auto& attrib : attribs){
        setAttribute(index++, attrib);
    }
    BindVertexBuffer(currentVBO);
    BindVertexArray(currentVAO);
}

auto Mesh::EnableAttribs() const -> void
{
    auto currentVAO = CurrentVAO();
    auto currentVBO = CurrentVBO();

    if(currentVAO == VAO){
        for(GLuint i = 0; i < attribs.size(); i++){
            glEnableVertexAttribArray(i);
        }
    }else{   
        BindVertexArray(VAO);
        BindVertexBuffer(VBO);
        
        for(GLuint i = 0; i < attribs.size(); i++){
            glEnableVertexAttribArray(i);
        }
        
        BindVertexBuffer(currentVBO);
        BindVertexArray(currentVAO);
    }
}

auto Mesh::DisableAttribs() const -> void
{
    auto currentVAO = CurrentVAO();
    auto currentVBO = CurrentVBO();

    BindVertexArray(VAO);
    BindVertexBuffer(VBO);
    for(GLuint i = 0; i < attribs.size(); i++){
        glDisableVertexAttribArray(i);
    }
    BindVertexBuffer(currentVBO);
    BindVertexArray(currentVAO);
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

auto Mesh::Updata(GLuint buffer, const std::vector<VetexData>& vrtx) -> void
{
    auto currentVBO = CurrentVBO();
    BindVertexBuffer(buffer);
    glBufferData(GL_ARRAY_BUFFER, vrtx.size() * sizeof(Mesh::VetexData), vrtx.data(), GL_STATIC_DRAW);
    BindVertexBuffer(currentVBO);
}

auto Mesh::GenVertexArray() -> GLuint
{
    GLuint result = 0;
    glGenVertexArrays(1, &result);
    Info("GenVertexArray {}", result);
    return result;
}

auto Mesh::GenBuffer() -> GLuint
{
    GLuint result = 0;
    glGenBuffers(1, &result);
    Info("GenBuffer {}", result);
    return result;
}
auto Mesh::BindVertexArray(GLuint vao) -> void
{
    glBindVertexArray(vao);
}

auto Mesh::BindBuffer(GLenum type, GLuint buffer) -> void
{
    glBindBuffer(type, buffer);
}

auto Mesh::BindVertexBuffer(GLuint buffer) -> void
{
    BindBuffer(GL_ARRAY_BUFFER, buffer);
}

auto Mesh::BindIndexBuffer(GLuint buffer) -> void
{
    BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

auto Mesh::PrepareVertexdata() -> void
{
    Updata(VBO, this->vertices);
}

auto Mesh::Bind() const -> void
{
    EnableAttribs();
    BindVertexArray(this->VAO);
}

auto Mesh::VextexSize() const noexcept -> GLsizei
{
    return static_cast<GLsizei>(vertices.size());
}
