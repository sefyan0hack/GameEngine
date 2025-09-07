#pragma once
#include <format>
#include <string>
#include <vector>

#include "gl.h"
#include <glm/glm.hpp>

#include <cmrc/cmrc.hpp>

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
  GLsizei offset;
  GLint divisor;
};

class Material;

class Mesh
{
public:
  using VetexData = Vertex;

public:
    friend struct std::formatter<Mesh>;
    Mesh(const std::vector<VetexData> &vertices, std::string Name = std::format("Mesh{}", Count));
    ~Mesh() = default;

    Mesh(const Mesh& other) = delete;
    auto operator=(const Mesh& other) -> Mesh& = delete;

    Mesh(Mesh&& other) noexcept;
    auto operator=(Mesh&& other) noexcept -> Mesh&;

    auto operator==(const Mesh& other) const -> bool;

    auto set_attribute(GLuint index, AttributeInfo att) -> void;
    auto enable_attribs() const -> void;
    auto vextex_size() const noexcept -> GLsizei;

    static auto current_vao() -> GLuint;
    static auto current_vbo() -> GLuint;
    static auto flip_faces(std::vector<Vertex> verts) -> std::vector<Vertex>;
    private:
        auto clone_buffer(GLenum type, GLuint src) -> GLuint;
        auto clone_vbo(GLuint src) -> GLuint;
        auto prepare_attribs() ->void;
        auto updata() -> void;
        static auto gen_vertexarray() -> GLuint;
        static auto gen_buffer() -> GLuint;
        static auto bind_buffer(GLenum type, GLuint buffer) -> void;
        auto bind_vao() -> void;
        auto bind_vbo() -> void;
        // auto bind_ibo() -> void;
  public:
    std::string name;
    std::vector<VetexData> vertices;
    std::vector<AttributeInfo> attribs;
    GLuint VBO, VAO;
    inline static std::size_t Count = 1;
    inline static std::vector<Vertex> CUBE {
      // Front
      {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
      {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
    
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
      {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
      {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
    
      // Back
      {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
      {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
      {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
    
      {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
      {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
    
      // Left
      {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
      {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
      {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
    
      {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
      {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
    
      // Right
      {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
      {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
      {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
    
      {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
      {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
      {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
    
      // Top
      {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
      {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
    
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
      {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
      {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
    
      // Bottom
      {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
      {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
      {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
    
      {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
      {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
      {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
    };
    
    FOR_TEST
};

// custom Mesh Format
template<>
struct std::formatter<Mesh> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mesh& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "name": "{}", "VAO": {}, "VBO": {}, "verticesSize": {} }})"
    , obj.name, obj.VAO, obj.VBO, obj.vextex_size());
  }
};

auto obj_to_mesh(const char* filename) -> std::vector<Vertex>;
auto obj_to_mesh(cmrc::file src) -> std::vector<Vertex>;