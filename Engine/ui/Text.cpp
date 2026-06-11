#include "Text.hpp"
#include "gl.hpp"
#include <core/Exception.hpp>
#include <core/res.hpp>
#include <core/Log.hpp>

#include "Window.hpp"
#include "OpenGL.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"

#include <emath/emath.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#include <bit>
#include <string>
#include <vector>

Text::Text(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_Vert(std::make_shared<Shader>(res::get("res/Shaders/text.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(res::get("res/Shaders/text.frag"), GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
{
    // Create initial atlas
    create_atlas();
    
    // Initialize buffers
    init_buffers();
}

Text::~Text() {
    gl::DeleteTextures(1, &m_AtlasTexture);
    gl::DeleteVertexArrays(1, &VAO);
    gl::DeleteBuffers(1, &VBO);
    gl::DeleteBuffers(1, &EBO);
}

auto Text::init_buffers() -> void {
    // Generate and bind VAO
    gl::GenVertexArrays(1, &VAO);
    gl::BindVertexArray(VAO);

    // Generate VBO and EBO
    gl::GenBuffers(1, &VBO);
    gl::GenBuffers(1, &EBO);

    // Pre-allocate
    m_Vertices.reserve(MAX_VERTICES);
    m_Indices.reserve(MAX_INDICES);
    
    for (uint32_t i = 0; i < MAX_QUADS; ++i) {
        uint32_t offset = i * VERTICES_PER_QUAD;
        m_Indices.insert(m_Indices.end(), {
            offset + 0, offset + 1, offset + 2,
            offset + 0, offset + 2, offset + 3
        });
    }

    // Upload indices to EBO
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), m_Indices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes for batched rendering
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // Position - CHANGED TO 2 FLOATS (2D)
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // Texture coordinates
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    gl::BindVertexArray(0);
}

auto Text::create_atlas() -> void {
    auto font_data = res::get(FONT_NAME);

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, reinterpret_cast<const unsigned char*>(font_data.data()), 0)) {
        throw Exception("ERROR::STB_TRUETYPE: Failed to load font");
    }

    float scale = stbtt_ScaleForPixelHeight(&info, FONT_SIZE);

    int32_t ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    m_Ascent = static_cast<int32_t>(static_cast<float>(ascent) * scale);

    std::vector<unsigned char> bitmap(ATLAS_WIDTH * ATLAS_HEIGHT, 0);

    std::vector<stbtt_bakedchar> bakedChars(CHAR_COUNT);

    int result = stbtt_BakeFontBitmap(
        reinterpret_cast<const unsigned char*>(font_data.data()), 0,
        FONT_SIZE,
        bitmap.data(), ATLAS_WIDTH, ATLAS_HEIGHT,
        FIRST_GLYPH, CHAR_COUNT,
        bakedChars.data()
    );

    if (result <= 0) {
        throw Exception("ERROR::STB_TRUETYPE: Failed to bake font bitmap");
    }

    gl::GenTextures(1, &m_AtlasTexture);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasTexture);
    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (uint32_t  i = 0; i < CHAR_COUNT; ++i) {
        const auto& bc = bakedChars[i];
        int charcode = FIRST_GLYPH + i;

        int width = bc.x1 - bc.x0;
        int height = bc.y1 - bc.y0;

        m_Glyphs[charcode] = AtlasGlyph{
            emath::ivec2(width, height),
            emath::ivec2(static_cast<int>(bc.xoff + 0.5f), static_cast<int>(bc.yoff + 0.5f)),
            emath::vec2(bc.x0 / static_cast<float>(ATLAS_WIDTH), bc.y0 / static_cast<float>(ATLAS_HEIGHT)),
            emath::vec2(bc.x1 / static_cast<float>(ATLAS_WIDTH), bc.y1 / static_cast<float>(ATLAS_HEIGHT)),
            static_cast<uint32_t>(bc.xadvance * 64.0f)
        };
    }
}

auto Text::render() -> void {
    if (m_Batches.empty()) return;

    auto [width, height] = m_GApi.window().dims();
    auto projection = emath::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);

    m_Program->use();
    m_Program->set_uniform("u_Projection", projection);
    m_Program->set_uniform("u_Color", FONT_COLOR);
    m_Program->set_uniform("u_Texture", 0);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasTexture);

    gl::BindVertexArray(VAO);

    m_Vertices.clear();
    m_IndexCount = 0;
    for (const auto& [pos, text] : m_Batches) {
        float startX = pos.x;
        float x = startX;
        float y = static_cast<float>(height) - pos.y - static_cast<float>(m_Ascent);

        for (auto c : text) {
            auto glyph = m_Glyphs.at('?');
            try { glyph = m_Glyphs.at(c); } catch (...) {}

            float w = static_cast<float>(glyph.size.x);
            float h = static_cast<float>(glyph.size.y);

            if (x + w >= static_cast<float>(width)) {
                x = startX;
                y -= FONT_SIZE;
            }

            float xpos = x + static_cast<float>(glyph.bearing.x);
            float ypos = y - h - static_cast<float>(glyph.bearing.y);

            push_quad({xpos, ypos}, {w, h}, glyph.topleft, glyph.botright);

            x += static_cast<float>(glyph.advance >> 6);

            if (m_IndexCount >= MAX_INDICES) {
                flush_batch();
                m_Vertices.clear();
                m_IndexCount = 0;
            }
        }
    }

    // Flush remaining vertices
    if (m_IndexCount > 0) {
        flush_batch();
    }

    gl::DepthFunc(GL_LEQUAL);
    gl::DepthMask(GL_TRUE);
}

auto Text::flush_batch() -> void {
    if (m_Vertices.empty()) return;

    // Upload vertices to GPU
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferSubData(GL_ARRAY_BUFFER, 0, m_Vertices.size() * sizeof(Vertex), m_Vertices.data());

    // Draw
    gl::DrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);

}

auto Text::push_quad(const emath::vec2& position, const emath::vec2& size, const emath::vec2& texMin, const emath::vec2& texMax) -> void {
    Vertex vertices[4] = {
        {emath::vec2(position.x, position.y),                 emath::vec2(texMin.x, texMax.y)},
        {emath::vec2(position.x + size.x, position.y),        emath::vec2(texMax.x, texMax.y)},
        {emath::vec2(position.x + size.x, position.y + size.y), emath::vec2(texMax.x, texMin.y)},
        {emath::vec2(position.x, position.y + size.y),        emath::vec2(texMin.x, texMin.y)}
    };

    m_Vertices.insert(m_Vertices.end(), vertices, vertices + 4);
    m_IndexCount += INDICES_PER_QUAD;
}

auto Text::text(std::string text, emath::vec2 pos) -> void {
    m_Batches[pos] = text;
}

auto Text::print(std::string text) -> void {
    static emath::vec2 currs = {0, 0};
    m_Batches[currs] = text;
    currs += emath::vec2{0, FONT_SIZE};
}