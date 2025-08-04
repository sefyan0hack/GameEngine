#include <core/Mesh.hpp>
#include <core/Log.hpp>

namespace {
    constinit static AttributeInfo position {
        .size = 3,//decltype(Mesh::VetexData::Position)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = offsetof(Mesh::VetexData, Position),  // No reinterpret_cast!
        .divisor = 0,
    };
    
    constinit static AttributeInfo normals {
        .size = 3,//decltype(Mesh::VetexData::Normal)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = offsetof(Mesh::VetexData, Normal),  // No reinterpret_cast!
        .divisor = 0,
    };

    constinit static AttributeInfo texCoords {
        .size = 2,//decltype(Mesh::VetexData::TexCoords)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = offsetof(Mesh::VetexData, TexCoords),  // No reinterpret_cast!
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
        gl::GenBuffers(1, &EBO);
        gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
#endif

    PrepareVertexdata();
    PrepareAttribs();
    Count++;
    
}


Mesh::~Mesh()
{
    Count--;
    gl::DeleteBuffers(1, &EBO);
    gl::DeleteBuffers(1, &VBO);
    gl::DeleteVertexArrays(1, &VAO);
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
    if(this != &other){
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
    : name(std::exchange(other.name, {}))
    , vertices(std::exchange(other.vertices, {}))
    , attribs(std::exchange(other.attribs, {}))
    , VBO(std::exchange(other.VBO, 0))
    , EBO(std::exchange(other.EBO, 0))
    , VAO(std::exchange(other.VAO, 0))
{
}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh&
{
    if(this != &other){
        this->name = std::exchange(other.name, {});
        this->vertices = std::exchange(other.vertices, {});
        this->attribs = std::exchange(other.attribs, {});
        this->VBO = std::exchange(other.VBO, 0);
        this->EBO = std::exchange(other.EBO, 0);
        this->VAO = std::exchange(other.VAO, 0);
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
//     gl::GenVertexArrays(1, &clone);
//     if(clone == 0) throw CException("clone is 0");
    
//     GLint maxAttributes = 0;
//     gl::GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);

//     if(attribs.size() >= maxAttributes) throw CException("attribs reched max `{}`", maxAttributes);

//     // gl::BindVertexArray(src);

//     // std::vector<AttributeInfo> attributes(attribs);
//     // for (GLuint atrInd = 0; atrInd < attribs; ++atrInd)
//     // {
//     //     gl::GetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &attributes[atrInd].enabled);

//     //     if(attributes[atrInd].enabled){
//     //         gl::GetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_SIZE, &attributes[atrInd].size);
//     //         gl::GetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&attributes[atrInd].type);
//     //         gl::GetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, (GLint*)&attributes[atrInd].normalized);
//     //         gl::GetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &attributes[atrInd].stride);
//     //         gl::GetVertexAttribiv(atrInd, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &attributes[atrInd].divisor);
//     //         gl::GetVertexAttribPointerv(atrInd, GL_VERTEX_ATTRIB_ARRAY_POINTER, &attributes[atrInd].offset);
//     //     }
//     // }

//     gl::BindVertexArray(clone);
//     gl::BindBuffer(GL_ARRAY_BUFFER, this->VBO);
//     for (auto atrb : attribs)
//     {
//         setAttribute(atrb);
//     }
//     // gl::BindVertexArray(0);
//     // gl::BindBuffer(GL_ARRAY_BUFFER, 0);

//     return clone;
// }
auto Mesh::CloneBuffer(GLenum type, GLuint src) -> GLuint
{
    GLuint clone = GenBuffer();
    if(clone == 0) throw CException("VBO clone is 0");

    BindBuffer(type, src);
    
    GLint bufferSize = 0, usage = 0;
    gl::GetBufferParameteriv(type, GL_BUFFER_SIZE, &bufferSize);
    gl::GetBufferParameteriv(type, GL_BUFFER_USAGE, &usage);
    
    if (bufferSize > 0) {
        BindBuffer(GL_COPY_WRITE_BUFFER, clone);
        gl::BufferData(GL_COPY_WRITE_BUFFER, bufferSize, nullptr, static_cast<GLenum>(usage));
        gl::CopyBufferSubData(type, GL_COPY_WRITE_BUFFER, 0, 0, bufferSize);
    } else {
        gl::DeleteBuffers(1, &clone);
        throw CException("VBO bufferSize is 0");
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

    gl::VertexAttribPointer(
        index,
        att.size,
        att.type,
        att.normalized,
        att.stride,
        reinterpret_cast<GLvoid*>(att.offset)
    );

    gl::VertexAttribDivisor(index, static_cast<GLuint>(att.divisor));
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
            gl::EnableVertexAttribArray(i);
        }
    }else{   
        BindVertexArray(VAO);
        BindVertexBuffer(VBO);
        
        for(GLuint i = 0; i < attribs.size(); i++){
            gl::EnableVertexAttribArray(i);
        }
        
        BindVertexBuffer(currentVBO);
        BindVertexArray(currentVAO);
    }
}


auto Mesh::CurrentVAO() -> GLuint
{
    GLint currentVAO = 0;
    gl::GetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    return static_cast<GLuint>(currentVAO);
}

auto Mesh::CurrentVBO() -> GLuint
{
    GLint currentVBO = 0;
    gl::GetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
    return static_cast<GLuint>(currentVBO);
}

auto Mesh::Updata(GLuint buffer, const std::vector<VetexData>& vrtx) -> void
{
    auto currentVBO = CurrentVBO();
    BindVertexBuffer(buffer);
    gl::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vrtx.size() * sizeof(Mesh::VetexData)), vrtx.data(), GL_STATIC_DRAW);
    BindVertexBuffer(currentVBO);
}

auto Mesh::GenVertexArray() -> GLuint
{
    GLuint result = 0;
    gl::GenVertexArrays(1, &result);
    Info("GenVertexArray {}", result);
    return result;
}

auto Mesh::GenBuffer() -> GLuint
{
    GLuint result = 0;
    gl::GenBuffers(1, &result);
    Info("GenBuffer {}", result);
    return result;
}
auto Mesh::BindVertexArray(GLuint vao) -> void
{
    gl::BindVertexArray(vao);
}

auto Mesh::BindBuffer(GLenum type, GLuint buffer) -> void
{
    gl::BindBuffer(type, buffer);
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
