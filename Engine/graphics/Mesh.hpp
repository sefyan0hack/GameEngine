#pragma once
#include <format>
#include <string>
#include <vector>
#include <span>

#include "gl.hpp"

#include <emath/vec2.hpp>
#include <emath/vec3.hpp>

#include <cmrc/cmrc.hpp>
#include <engine_export.h>

struct ENGINE_EXPORT Vertex
{
  emath::vec3 Position;
  emath::vec3 Normal;
  emath::vec2 TexCoords;
};

struct ENGINE_EXPORT Attribute {
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLint stride;
  GLsizei offset;
  GLint divisor;
};

class Material;

class ENGINE_EXPORT Mesh
{
public:
  using VetexData = Vertex;

public:
    friend struct std::formatter<Mesh>;
    Mesh(const std::vector<VetexData> &vertices);
    
    Mesh(const Mesh& other) = delete;
    auto operator=(const Mesh& other) -> Mesh& = delete;
    
    Mesh(Mesh&& other) noexcept;
    auto operator=(Mesh&& other) noexcept -> Mesh&;
    
    ~Mesh();

    auto set_attribute(GLuint index, Attribute att) -> void;
    auto vertex_size() const noexcept -> GLsizei;

    static auto flip_faces(std::vector<Vertex> verts) -> std::vector<Vertex>;
    static auto CUBE() -> std::vector<Vertex>;

  public:
    std::vector<VetexData> m_Vertices;
    std::vector<Attribute> m_Attribs;
    GLuint VAO, VBO;
};

ENGINE_EXPORT auto obj_to_mesh(const char* filename) -> std::vector<Vertex>;
ENGINE_EXPORT auto obj_to_mesh(std::span<const char> src) -> std::vector<Vertex>;

#ifdef __cpp_lib_formatters
// custom Mesh Format
template<>
struct std::formatter<Mesh> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mesh& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "VAO": {}, "VBO": {}, "verticesSize": {} }})"
    , obj.VAO, obj.VBO, obj.vertex_size());
  }
};
#endif