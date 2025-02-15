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
  GLint enabled;
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
    friend struct std::formatter<Mesh>;
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices = {}, std::string Name = std::format("Mesh{}", Count));
    // Mesh(const std::vector<GLfloat> vertices, std::string Name = std::format("Mesh{}", Count));

    Mesh(const Mesh& other);
    auto operator=(const Mesh& other) -> Mesh&;

    Mesh(Mesh&& other) noexcept;
    auto operator=(Mesh&& other) noexcept -> Mesh&;

    auto operator==(const Mesh& other) const -> bool;

    ~Mesh();
    auto setAttribute(AttributeInfo att) -> void;
    private:
        auto CloneBuffer(GLenum type, GLuint src) -> GLuint;
        auto CloneVBO(GLuint src) -> GLuint;
        auto CloneEBO(GLuint src) -> GLuint;
  public:
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
    "Mesh: {{ name: {}, VAO: {}, VBO: {}, EBO: {}, verticesSize: {} }}"
    , obj.name, obj.VAO, obj.VBO, obj.EBO, obj.vInSize);
  }
};