#pragma once

#include <glm/glm.hpp>
#include <core/OpenGL.hpp>
#include <core/fmts.hpp>


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

    auto setAttribute(GLuint index, AttributeInfo att) -> void;
    auto EnableAttribs() const -> void;
    auto VextexSize() const noexcept -> GLsizei;

    static auto CurrentVAO() -> GLuint;
    static auto CurrentVBO() -> GLuint;
    private:
        auto CloneBuffer(GLenum type, GLuint src) -> GLuint;
        auto CloneVBO(GLuint src) -> GLuint;
        auto PrepareAttribs() ->void;
        static auto Updata(GLuint buffer, const std::vector<VetexData>& vrtx) -> void;
        static auto GenVertexArray() -> GLuint;
        static auto GenBuffer() -> GLuint;
        static auto BindVertexArray(GLuint vao) -> void;
        static auto BindBuffer(GLenum type, GLuint buffer) -> void;
        static auto BindVertexBuffer(GLuint buffer) -> void;
        static auto BindIndexBuffer(GLuint buffer) -> void;
  public:
    std::string name;
    std::vector<VetexData> vertices;
    std::vector<AttributeInfo> attribs;
    GLuint VBO, VAO;
    inline static std::size_t Count = 1;
    inline static std::vector<Vertex> CUBE {
      // Front Face
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top Left
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top Right
      {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom Right
      {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom Left

      // Back Face
      {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // Top Left
      {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // Bottom Right

      // Left Face
      {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // Top Left
      {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Right
      {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left

      // Right Face
      {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Left
      {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // Top Right
      {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Right
      {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Left
      {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Right
      {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Left

      // Top Face
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // Top Left
      {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
      {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Right
      {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left

      // Bottom Face
      {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // Top Left
      {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
      {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
      {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Right
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
    , obj.name, obj.VAO, obj.VBO, obj.VextexSize());
  }
};

inline auto Obj2Mesh(const char* filename) -> std::vector<Vertex>
{
  std::ifstream file(filename);

  if(!file) throw CException("Error: Unable to open {}", filename);
  std::vector<glm::vec3> positions;      // Store raw vertex positions
  std::vector<Vertex> vertices_output;    // Final vertex output

  std::string line;
  while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string type;
      iss >> type;

      if (type == "v") {  // Vertex position
          float x, y, z;
          iss >> x >> y >> z;
          positions.push_back(glm::vec3(x, y, z));
      }
      else if (type == "f") {  // Face definition
          std::vector<size_t> faceIndices;
          std::string token;
          
          while (iss >> token) {
              // Extract vertex index part (ignore normals/textures after '/')
              size_t pos = token.find('/');
              if (pos != std::string::npos) {
                  token = token.substr(0, pos);
              }
              
              if (token.empty()) continue;
              
              // Convert to index and validate
              int idx = std::stoi(token);
              int vertexCount = static_cast<int>(positions.size());
              
              // Handle negative indices (relative positions)
              if (idx < 0) idx = vertexCount + idx + 1;
              
              // Convert to 0-based index and validate bounds
              idx--;
              if (idx < 0 || idx >= vertexCount) continue;
              
              faceIndices.push_back(static_cast<size_t>(idx));
          }

          // Triangulate face (convert polygon to triangles)
          if (faceIndices.size() < 3) continue;
          
          for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
              // Create vertices for current triangle
              Vertex v1, v2, v3;
              
              // Set positions only (ignore normals/texcoords)
              v1.Position = positions[faceIndices[0]];
              v2.Position = positions[faceIndices[i]];
              v3.Position = positions[faceIndices[i + 1]];
              
              // Set normals and texcoords to zero
              v1.Normal = v2.Normal = v3.Normal = glm::vec3(0.0f);
              v1.TexCoords = v2.TexCoords = v3.TexCoords = glm::vec2(0.0f);
              
              // Add to output
              vertices_output.push_back(v1);
              vertices_output.push_back(v2);
              vertices_output.push_back(v3);
          }
      }
  }
  
  return vertices_output;
}

// inline auto Obj2Mesh(const char* filename) -> Mesh
// {
//   std::ifstream file(filename);

//   if(!file) throw CException("Error: Unable to open {}", filename);

//   auto verticies = std::vector<Vertex>{};
//   std::string line;

//   while (std::getline(file, line)) {
//     if(line.starts_with("v  ")){
//       auto verts = utils::split(line.c_str() + 3, " ");

//       verticies.push_back(
//         Vertex { .Position = { std::stof(verts[0]), std::stof(verts[1]), std::stof(verts[2])} }
//       );
//     }
//   }
//   return verticies;
// }