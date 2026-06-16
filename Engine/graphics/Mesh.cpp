#include "Mesh.hpp"
#include "OpenGL.hpp"
#include <core/Log.hpp>
#include <core/Exception.hpp>

#include <fstream>
#include <sstream>

auto Mesh::CUBE() -> std::vector<Vertex>{

    static auto _ = std::vector<Vertex> {
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
    return _;
}

namespace {
    constinit static Attribute position {
        .size = 3,//decltype(Mesh::VetexData::Position)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = offsetof(Mesh::VetexData, Position),  // No reinterpret_cast!
        .divisor = 0,
    };
    
    constinit static Attribute normals {
        .size = 3,//decltype(Mesh::VetexData::Normal)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = offsetof(Mesh::VetexData, Normal),  // No reinterpret_cast!
        .divisor = 0,
    };

    constinit static Attribute texCoords {
        .size = 2,//decltype(Mesh::VetexData::TexCoords)::length(),
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::VetexData),
        .offset = offsetof(Mesh::VetexData, TexCoords),  // No reinterpret_cast!
        .divisor = 0,
    };
}

Mesh::Mesh(const std::vector<Vertex> &vertices)
    : m_Vertices(vertices)
    , m_Attribs({position, normals, texCoords})
    , VAO(0)
    , VBO(0)
{
    gl::GenVertexArrays(1, &VAO);
    gl::GenBuffers(1, &VBO);

    gl::BindVertexArray(VAO);
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    
    gl::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_Vertices.size() * sizeof(Mesh::VetexData)), m_Vertices.data(), GL_STATIC_DRAW);

    GLuint index = 0;
    for(const auto& attrib : m_Attribs){
        set_attribute(index++, attrib);
    }
}

Mesh::Mesh(Mesh &&other) noexcept
    : m_Vertices(std::move(other.m_Vertices))
    , m_Attribs(std::move(other.m_Attribs))
    , VAO(other.VAO)
    , VBO(other.VBO)
{
    other.VAO = 0;
    other.VBO = 0;
}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh &
{
    if(this != &other){
        gl::DeleteBuffers(1, &VBO);
        gl::DeleteVertexArrays(1, &VAO);

        m_Vertices = std::move(other.m_Vertices);
        m_Attribs = std::move(other.m_Attribs);

        VAO = other.VAO;
        VBO = other.VBO;

        other.VAO = 0;
        other.VBO = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    gl::DeleteBuffers(1, &VBO);
    gl::DeleteVertexArrays(1, &VAO);
}

auto Mesh::set_attribute(GLuint index, Attribute att) -> void
{
    Expect(att.size > 0 && att.size <= 4, "position.size : 0<{}<4 wrong", att.size);

    gl::VertexAttribPointer(
        index,
        att.size,
        att.type,
        att.normalized,
        att.stride,
        reinterpret_cast<void*>(static_cast<uintptr_t>(att.offset))
    );
    gl::EnableVertexAttribArray(index);
    gl::VertexAttribDivisor(index, static_cast<GLuint>(att.divisor));
}

auto Mesh::vertex_size() const noexcept -> GLsizei
{
    return static_cast<GLsizei>(m_Vertices.size());
}

auto Mesh::flip_faces(std::vector<Vertex> verts) -> std::vector<Vertex>
{
    for (size_t i = 0; i + 2 < verts.size(); i += 3) {
      std::swap(verts[i + 1], verts[i + 2]);
    }
    return verts;
}

auto obj_to_mesh(const char* filename) -> std::vector<Vertex>
{
  std::ifstream file(filename);

  if(!file) throw Exception("Error: Unable to open {}", filename);
  std::vector<emath::vec3> positions;      // Store raw vertex positions
  std::vector<Vertex> vertices_output;    // Final vertex output

  std::string line;
  while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string type;
      iss >> type;

      if (type == "v") {  // Vertex position
          float x, y, z;
          iss >> x >> y >> z;
          positions.push_back(emath::vec3(x, y, z));
      }
      else if (type == "f") {  // Face definition
          std::vector<size_t> faceIndices;
          std::string token;
          
          while (iss >> token) {
              // Extract vertex index part (ignore normals/textures after '/')
              size_t pos = token.find('/');
              if (pos != std::string::npos) {
                  token = token.substr(0, pos); // performance: Ineffective call of function 'substr' because a prefix of the string is assigned to itself. Use resize() or pop_back() instead. [uselessCallsSubstr]
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
              v1.Normal = v2.Normal = v3.Normal = emath::vec3(0.0f);
              v1.TexCoords = v2.TexCoords = v3.TexCoords = emath::vec2(0.0f);
              
              // Add to output
              vertices_output.push_back(v1);
              vertices_output.push_back(v2);
              vertices_output.push_back(v3);
          }
      }
  }
  
  return vertices_output;
}


auto obj_to_mesh(std::span<const char> src) -> std::vector<Vertex>
{
  std::string_view sv(src.data(), src.size());
  std::vector<emath::vec3> positions;
  std::vector<Vertex> vertices_output;

  auto trim = [](std::string_view s) -> std::string_view {
    size_t b = 0;
    while (b < s.size() && (s[b] == ' ' || s[b] == '\t')) ++b;
    size_t e = s.size();
    while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t')) --e;
    return s.substr(b, e - b);
  };

  size_t i = 0;
  while (i < sv.size()) {
    // find end-of-line (handles '\n' or '\r' or "\r\n")
    size_t j = sv.find_first_of("\r\n", i);
    std::string_view line = (j == sv.npos) ? sv.substr(i) : sv.substr(i, j - i);
    // advance i past the newline(s)
    if (j == sv.npos) {
      i = sv.size();
    } else {
      i = j + 1;
      // if we found '\r' then skip a following '\n'
      if (sv[j] == '\r' && i < sv.size() && sv[i] == '\n') ++i;
    }

    auto tline = trim(line);
    if (tline.empty() || tline[0] == '#') continue; // comment or empty

    // we'll parse using istringstream on a temporary std::string (simple & robust)
    std::string s(tline);
    std::istringstream iss(s);
    std::string type;
    iss >> type;

    if (type == "v") {
      float x = 0.f, y = 0.f, z = 0.f;
      iss >> x >> y >> z;
      positions.emplace_back(x, y, z);
    }
    else if (type == "f") {
      std::vector<size_t> faceIndices;
      std::string token;
      while (iss >> token) {
        // remove anything after the first '/'
        const size_t posSlash = token.find('/');
        if (posSlash != std::string::npos) token = token.substr(0, posSlash);
        if (token.empty()) continue;

        // stoi is fine here (OBJ indices are small); handle negative (relative) indices
        int idx = 0;
        try {
          idx = std::stoi(token);
        } catch (...) {
          continue; // ignore malformed index
        }

        int vertexCount = static_cast<int>(positions.size());
        if (idx < 0) idx = vertexCount + idx + 1; // relative indexing
        idx -= 1; // convert to 0-based
        if (idx < 0 || idx >= vertexCount) continue;
        faceIndices.push_back(static_cast<size_t>(idx));
      }

      // triangulate polygon (fan)
      if (faceIndices.size() < 3) continue;
      for (size_t k = 1; k + 1 < faceIndices.size(); ++k) {
        Vertex v1{}, v2{}, v3{};
        v1.Position = positions[faceIndices[0]];
        v2.Position = positions[faceIndices[k]];
        v3.Position = positions[faceIndices[k + 1]];

        v1.Normal = v2.Normal = v3.Normal = emath::vec3(0.0f);
        v1.TexCoords = v2.TexCoords = v3.TexCoords = emath::vec2(0.0f);

        vertices_output.push_back(v1);
        vertices_output.push_back(v2);
        vertices_output.push_back(v3);
      }
    }
    // ignore other directive types (vn, vt, etc.) for now
  }
  return vertices_output;
}
