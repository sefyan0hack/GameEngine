#pragma once
#include <format>
#include <vector>

#include <emath/vec2.hpp>
#include <emath/vec3.hpp>

#include <engine_export.h>

class Material;

class ENGINE_EXPORT Mesh
{
public:
  struct Vertex
  {
    emath::vec3 Position;
    emath::vec3 Normal;
    emath::vec2 TexCoords;
  };

public:
    friend struct std::formatter<Mesh>;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
    Mesh(std::pair<std::vector<Vertex>, std::vector<uint16_t>> vert_inds);
    
    Mesh(const Mesh& other) = delete;
    auto operator=(const Mesh& other) -> Mesh& = delete;
    
    Mesh(Mesh&& other) noexcept;
    auto operator=(Mesh&& other) noexcept -> Mesh&;

    ~Mesh();

    auto vertex_size() const noexcept -> size_t;
    auto indices_size() const noexcept -> size_t;

    static auto flip_faces(std::vector<Vertex> verts) -> std::vector<Vertex>;
  
    static auto CUBE_VERTICES() -> std::vector<Vertex>;
    static auto CUBE_INDICES() -> std::vector<uint16_t>;

  public:
    std::vector<Vertex> m_Vertices;
    std::vector<uint16_t> m_Indices;
    uint32_t VAO, VBO, IBO;
};

ENGINE_EXPORT std::pair<std::vector<Mesh::Vertex>, std::vector<uint16_t>> load_obj(const char* obj);

#ifdef __cpp_lib_formatters
// custom Mesh Format
template<>
struct std::formatter<Mesh> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mesh& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "VAO": {}, "VBO": {}, "vertices": {}, "indices": {} }})"
    , obj.VAO, obj.VBO, obj.vertex_size(), obj.indices_size());
  }
};
#endif