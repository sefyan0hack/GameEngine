#include <core/Log.hpp>
#include "Mesh.hpp"
#include "OpenGL.hpp"

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

Mesh::Mesh(const std::vector<Vertex> &vertices, std::string Name)
    : name(Name)
    , vertices(vertices)
    , attribs({position, normals, texCoords})
    , VBO(gen_buffer())
    , VAO(gen_vertexarray())
{
    Count++;

    bind_vao();
    bind_vbo();
    updata();
    enable_attribs();
    prepare_attribs();

    CTOR_LOG
}

Mesh::~Mesh()
{
    gl::DeleteBuffers(1, &VBO);
    gl::DeleteVertexArrays(1, &VAO);
    Count--;
    DTOR_LOG
}

auto Mesh::clone_buffer(GLenum type, GLuint src) -> GLuint
{
    GLuint clone = gen_buffer();
    if(clone == 0) throw Exception("VBO clone is 0");

    bind_buffer(type, src);
    
    GLint bufferSize = 0, usage = 0;
    gl::GetBufferParameteriv(type, GL_BUFFER_SIZE, &bufferSize);
    gl::GetBufferParameteriv(type, GL_BUFFER_USAGE, &usage);
    
    if (bufferSize > 0) {
        bind_buffer(GL_COPY_WRITE_BUFFER, clone);
        gl::BufferData(GL_COPY_WRITE_BUFFER, bufferSize, nullptr, static_cast<GLenum>(usage));
        gl::CopyBufferSubData(type, GL_COPY_WRITE_BUFFER, 0, 0, bufferSize);
    } else {
        gl::DeleteBuffers(1, &clone);
        throw Exception("VBO bufferSize is 0");
    }

    return clone;
}

auto Mesh::clone_vbo(GLuint src) -> GLuint
{
    return clone_buffer(GL_ARRAY_BUFFER, src);
}

auto Mesh::set_attribute(GLuint index, AttributeInfo att) -> void
{
    Expect(att.size > 0 && att.size <= 4, "position.size : 0<{}<4 wrong", att.size);

    debug::log(
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
        reinterpret_cast<void*>(static_cast<uintptr_t>(att.offset))
    );

    gl::VertexAttribDivisor(index, static_cast<GLuint>(att.divisor));
}

auto Mesh::prepare_attribs() -> void
{
    GLuint index = 0;
    for(const auto& attrib : attribs){
        set_attribute(index++, attrib);
    }
}

auto Mesh::enable_attribs() const -> void
{
    for(GLuint i = 0; i < attribs.size(); i++){
        gl::EnableVertexAttribArray(i);
    }
}


auto Mesh::current_vao() -> GLuint
{
    GLint currentVAO = 0;
    gl::GetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    return static_cast<GLuint>(currentVAO);
}

auto Mesh::current_vbo() -> GLuint
{
    GLint currentVBO = 0;
    gl::GetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
    return static_cast<GLuint>(currentVBO);
}

auto Mesh::updata() -> void
{
    gl::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Mesh::VetexData)), vertices.data(), GL_STATIC_DRAW);
}

auto Mesh::gen_vertexarray() -> GLuint
{
    GLuint result = 0;
    gl::GenVertexArrays(1, &result);
    debug::log("GenVertexArray {}", result);
    return result;
}

auto Mesh::gen_buffer() -> GLuint
{
    GLuint result = 0;
    gl::GenBuffers(1, &result);
    debug::log("GenBuffer {}", result);
    return result;
}


auto Mesh::bind_buffer(GLenum type, GLuint buffer) -> void
{
    gl::BindBuffer(type, buffer);
}

auto Mesh::bind_vao() -> void
{
    gl::BindVertexArray(VAO);
}

auto Mesh::bind_vbo() -> void
{
    bind_buffer(GL_ARRAY_BUFFER, VBO);
}

// auto Mesh::bind_IBO() -> void
// {
//     bind_buffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
// }

auto Mesh::vextex_size() const noexcept -> GLsizei
{
    return static_cast<GLsizei>(vertices.size());
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


auto obj_to_mesh( cmrc::file src) -> std::vector<Vertex>
{
  auto begin = src.begin();
  auto end = src.end();

  std::string_view sv(begin, static_cast<size_t>(end - begin));
  std::vector<glm::vec3> positions;
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

        v1.Normal = v2.Normal = v3.Normal = glm::vec3(0.0f);
        v1.TexCoords = v2.TexCoords = v3.TexCoords = glm::vec2(0.0f);

        vertices_output.push_back(v1);
        vertices_output.push_back(v2);
        vertices_output.push_back(v3);
      }
    }
    // ignore other directive types (vn, vt, etc.) for now
  }
  return vertices_output;
}
