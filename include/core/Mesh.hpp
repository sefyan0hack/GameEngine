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

class Material;

class Mesh
{
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices = {});
    Mesh(const std::vector<GLfloat> vertices);
    ~Mesh();
    auto EnableAttribs() const -> void;
    auto DisableAttribs() const -> void;
public:
    GLuint VAO, VBO, EBO;
    GLsizei vInSize;
    GLuint attribs;
};

// custom Mesh Format
template<>
struct std::formatter<Mesh> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mesh& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "Mesh: {{ VAO: {}, VBO: {}, EBO: {}, verticesSize: {} }}"
    , obj.VAO, obj.VBO, obj.EBO, obj.vInSize);
  }
};