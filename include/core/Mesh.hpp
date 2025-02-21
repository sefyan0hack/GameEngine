#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <core/gl.h>
#include <core/fmts.hpp>
#include <format>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct AttributeInfo {
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLint stride;
  GLvoid* offset;
  GLint divisor;
};

class Material;

class Mesh
{
public:
  using VetexData = Vertex;

public:
    friend struct std::formatter<Mesh>;
    Mesh(const std::vector<VetexData> &vertices, const std::vector<GLuint> &indices = {}, std::string Name = std::format("Mesh{}", Count));
    // Mesh(const std::vector<GLfloat> vertices, std::string Name = std::format("Mesh{}", Count));

    Mesh(const Mesh& other);
    auto operator=(const Mesh& other) -> Mesh&;

    Mesh(Mesh&& other) noexcept;
    auto operator=(Mesh&& other) noexcept -> Mesh&;

    auto operator==(const Mesh& other) const -> bool;

    ~Mesh();
    auto setAttribute(GLuint index, AttributeInfo att) -> void;
    auto EnableAttribs() const -> void;
    auto DisableAttribs() const -> void;

    static auto CurrentVAO() -> GLuint;
    static auto CurrentVBO() -> GLuint;
    private:
        auto CloneBuffer(GLenum type, GLuint src) -> GLuint;
        auto CloneVBO(GLuint src) -> GLuint;
        auto CloneEBO(GLuint src) -> GLuint;
        auto PrepareAttribs() ->void;
        auto PrepareVertexdata() -> void;
        static auto Updata(GLuint buffer, const std::vector<VetexData>& vrtx) -> void;
        static auto GenVertexArray() -> GLuint;
        static auto GenBuffer() -> GLuint;
        static auto BindVertexArray(GLuint vao) -> void;
        static auto BindBuffer(GLenum type, GLuint buffer) -> void;
        static auto BindVertexBuffer(GLuint buffer) -> void;
        static auto BindIndexBuffer(GLuint buffer) -> void;
  public:
    std::vector<VetexData> vertices;
    GLsizei vInSize;
    std::vector<AttributeInfo> attribs;
    std::string name;
    GLuint VBO, EBO, VAO;
    inline static size_t Count = 1;
};

// custom Mesh Format
template<>
struct std::formatter<Mesh> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mesh& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "{}: {{ name: {}, VAO: {}, VBO: {}, EBO: {}, verticesSize: {} }}"
    , typeid(obj).name(), obj.name, obj.VAO, obj.VBO, obj.EBO, obj.vInSize);
  }
};