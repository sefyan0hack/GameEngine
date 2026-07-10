#include "Mesh.hpp"

#include "gl.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>
#include <core/res.hpp>

#include <cstdio>
#include <charconv>
#include <unordered_map>

auto Mesh::CUBE_VERTICES() -> std::vector<Mesh::Vertex>{

    static auto _ = std::vector<Mesh::Vertex> {
        // Front (+Z)
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

        // Back (-Z)
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},

        // Left (-X)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

        // Right (+X)
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},

        // Top (+Y)
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},

        // Bottom (-Y)
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
    };
    return _;
}

auto Mesh::CUBE_INDICES() -> std::vector<uint16_t> 
{
    static auto _ =   std::vector<uint16_t> {
        // Front
        0, 1, 2,
        2, 3, 0,
        // Back
        4, 5, 6,
        6, 7, 4,
        // Left
        8, 9,10,
        10,11, 8,
            // Right
        12,13,14,
        14,15,12,
            // Top
        16,17,18,
        18,19,16,
            // Bottom
        20,21,22,
        22,23,20,
    };

    return _;
}

namespace attribs {

    struct Attribute {
        int32_t size;
        uint32_t type;
        bool normalized;
        int32_t stride;
        int32_t offset;
        int32_t divisor;
    };

    constinit static Attribute position {
        .size = 3,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::Vertex),
        .offset = offsetof(Mesh::Vertex, Position),  // No reinterpret_cast!
        .divisor = 0,
    };
    
    constinit static Attribute normals {
        .size = 3,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::Vertex),
        .offset = offsetof(Mesh::Vertex, Normal),  // No reinterpret_cast!
        .divisor = 0,
    };

    constinit static Attribute texCoords {
        .size = 2,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Mesh::Vertex),
        .offset = offsetof(Mesh::Vertex, TexCoords),  // No reinterpret_cast!
        .divisor = 0,
    };

    auto set_attribute(uint32_t index, Attribute att) -> void
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
        gl::VertexAttribDivisor(index, static_cast<uint32_t>(att.divisor));
    }
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
    : m_Vertices(vertices)
    , m_Indices(indices)
    , VAO(0)
    , VBO(0)
    , IBO(0)
{
    gl::GenVertexArrays(1, &VAO);
    gl::GenBuffers(1, &VBO);
    gl::GenBuffers(1, &IBO);

    gl::BindVertexArray(VAO);

    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, static_cast<ptrdiff_t>(m_Vertices.size() * sizeof(Mesh::Vertex)), m_Vertices.data(), GL_STATIC_DRAW);
    
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<ptrdiff_t>(m_Indices.size() * sizeof(uint16_t)), m_Indices.data(), GL_STATIC_DRAW);
    
    uint32_t index = 0;
    for(auto a : {attribs::position, attribs::normals, attribs::texCoords}){
        attribs::set_attribute(index++, a);
    }
}

Mesh::Mesh(std::pair<std::vector<Vertex>, std::vector<uint16_t>> vert_inds)
    : Mesh(vert_inds.first, vert_inds.second)
{}

Mesh::Mesh(Mesh &&other) noexcept
    : m_Vertices(std::move(other.m_Vertices))
    , VAO(other.VAO)
    , VBO(other.VBO)
    , IBO(other.IBO)
{
    other.VAO = 0;
    other.VBO = 0;
    other.IBO = 0;
}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh &
{
    if(this != &other){
        gl::DeleteBuffers(1, &IBO);
        gl::DeleteBuffers(1, &VBO);
        gl::DeleteVertexArrays(1, &VAO);

        m_Vertices = std::move(other.m_Vertices);

        VAO = other.VAO;
        VBO = other.VBO;
        IBO = other.IBO;
        
        other.VAO = 0;
        other.VBO = 0;
        other.IBO = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    gl::DeleteBuffers(1, &IBO);
    gl::DeleteBuffers(1, &VBO);
    gl::DeleteVertexArrays(1, &VAO);
}

auto Mesh::vertex_size() const noexcept -> size_t
{
    return m_Vertices.size();
}

auto Mesh::indices_size() const noexcept -> size_t
{
    return m_Indices.size();
}

auto Mesh::flip_faces(std::vector<Mesh::Vertex> verts) -> std::vector<Mesh::Vertex>
{
    for (size_t i = 0; i + 2 < verts.size(); i += 3) {
      std::swap(verts[i + 1], verts[i + 2]);
    }
    return verts;
}

std::pair<std::vector<Mesh::Vertex>, std::vector<uint16_t>> load_obj(const char* obj)
{
    std::vector<emath::vec3> positions;
    std::vector<emath::vec2> texcoords;
    std::vector<emath::vec3> normals;

    std::vector<Mesh::Vertex> vertices;
    std::vector<uint16_t> indices;

    std::unordered_map<uint64_t, uint16_t> vertexMap;

    positions.reserve(10000);
    texcoords.reserve(10000);
    normals.reserve(10000);
    vertices.reserve(10000);
    indices.reserve(30000);

    auto objData = res::get(obj);
    const char* data = objData.data();
    size_t size = objData.size();
    size_t pos = 0;

    // Helper lambdas for parsing with std::from_chars + whitespace handling
    auto skip_space = [](const char*& p, const char* end) {
        while (p < end && std::isspace(static_cast<unsigned char>(*p)))
            ++p;
    };

    auto parse_float = [&](const char*& p, const char* end, float& val) -> bool {
        skip_space(p, end);
        if (p >= end) return false;
        auto [ptr, ec] = std::from_chars(p, end, val);
        if (ec != std::errc()) return false;
        p = ptr;
        return true;
    };

    while (pos < size) {
        // Find end of line
        size_t end = pos;
        while (end < size && data[end] != '\n') ++end;
        size_t lineLen = end - pos;

        // Skip possible '\r'
        if (lineLen > 0 && data[end-1] == '\r') lineLen--;

        // Skip empty lines and comments
        if (lineLen == 0 || data[pos] == '#') {
            pos = end + 1;
            continue;
        }

        char lineBuf[512];
        size_t copyLen = lineLen < sizeof(lineBuf)-1 ? lineLen : sizeof(lineBuf)-1;
        std::memcpy(lineBuf, data + pos, copyLen);
        lineBuf[copyLen] = '\0';

        const char* lineEnd = lineBuf + copyLen;  // end pointer for from_chars

        if (lineBuf[0] == 'v' && lineBuf[1] == ' ') {
            // "v  x  y  z"
            const char* p = lineBuf + 2;           // skip "v "
            float x, y, z;
            if (parse_float(p, lineEnd, x) &&
                parse_float(p, lineEnd, y) &&
                parse_float(p, lineEnd, z))
            {
                positions.push_back({x, y, z});
            }
        }
        else if (lineBuf[0] == 'v' && lineBuf[1] == 't') {
            // "vt  u  v"
            const char* p = lineBuf + 2;           // skip "vt"
            float u, v;
            if (parse_float(p, lineEnd, u) &&
                parse_float(p, lineEnd, v))
            {
                texcoords.push_back({u, v});
            }
        }
        else if (lineBuf[0] == 'v' && lineBuf[1] == 'n') {
            // "vn  nx  ny  nz"
            const char* p = lineBuf + 2;           // skip "vn"
            float nx, ny, nz;
            if (parse_float(p, lineEnd, nx) &&
                parse_float(p, lineEnd, ny) &&
                parse_float(p, lineEnd, nz))
            {
                normals.push_back({nx, ny, nz});
            }
        }
        else if (lineBuf[0] == 'f' && lineBuf[1] == ' ') {
            // Face parsing unchanged (uses strtoul)
            std::vector<uint32_t> pIdx, tIdx, nIdx;
            const char* p = lineBuf + 2;  // skip "f "
            while (*p) {
                while (*p == ' ') ++p;
                if (!*p) break;

                uint32_t pi = 0, ti = 0, ni = 0;
                pi = static_cast<uint32_t>(std::strtoul(p, const_cast<char**>(&p), 10));
                if (*p == '/') {
                    ++p;
                    if (*p != '/') {
                        ti = static_cast<uint32_t>(std::strtoul(p, const_cast<char**>(&p), 10));
                    }
                    if (*p == '/') {
                        ++p;
                        ni = static_cast<uint32_t>(std::strtoul(p, const_cast<char**>(&p), 10));
                    }
                }
                pIdx.push_back(pi);
                tIdx.push_back(ti);
                nIdx.push_back(ni);
            }

            if (pIdx.size() >= 3) {
                for (size_t i = 1; i + 1 < pIdx.size(); ++i) {
                    uint32_t tri[3][3] = {
                        { pIdx[0], tIdx[0], nIdx[0] },
                        { pIdx[i], tIdx[i], nIdx[i] },
                        { pIdx[i+1], tIdx[i+1], nIdx[i+1] }
                    };
                    for (int v = 0; v < 3; ++v) {
                        uint32_t pp = tri[v][0], t = tri[v][1], n = tri[v][2];
                        if (pp == 0 || pp > positions.size()) continue;
                        if (t > texcoords.size()) t = 0;
                        if (n > normals.size()) n = 0;

                        uint64_t key = (static_cast<uint64_t>(pp) << 40) |
                                       (static_cast<uint64_t>(t) << 20) |
                                        static_cast<uint64_t>(n);

                        auto it = vertexMap.find(key);
                        if (it != vertexMap.end()) {
                            indices.push_back(it->second);
                        } else {
                            Mesh::Vertex vert;
                            vert.Position = positions[pp - 1];
                            vert.TexCoords = (t > 0) ? texcoords[t - 1] : emath::vec2{0,0};
                            vert.Normal    = (n > 0) ? normals[n - 1]   : emath::vec3{0,0,1};

                            uint16_t idx = static_cast<uint16_t>(vertices.size());
                            vertices.push_back(vert);
                            indices.push_back(idx);
                            vertexMap[key] = idx;
                        }
                    }
                }
            }
        }
        pos = end + 1;
    }

    return { std::move(vertices), std::move(indices) };
}
