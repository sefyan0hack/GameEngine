#include "Text.hpp"
#include "gl.hpp"
#include <core/Exception.hpp>
#include <core/res.hpp>
#include <core/Log.hpp>

#include "Window.hpp"
#include "OpenGL.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <bit>
#include <string>

extern cmrc::embedded_filesystem embed_filesystem;

Text::Text(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_Vert(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/text.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/text.frag"), GL_FRAGMENT_SHADER))
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
    FT_Library ft;
    FT_Face face;

    if (FT_Init_FreeType(&ft)) {
        throw Exception("ERROR::FREETYPE: Could not init FreeType Library");
    }

    auto font_data = embed_filesystem.open(FONT_NAME);

    if (FT_New_Memory_Face(ft, std::bit_cast<FT_Byte const*>(font_data.begin()), static_cast<FT_Long>(font_data.size()), 0, &face)) {
        FT_Done_FreeType(ft);
        throw Exception("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

    // Get font metrics
    m_Ascent = face->size->metrics.ascender >> 6;

    // Create atlas texture
    gl::GenTextures(1, &m_AtlasTexture);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasTexture);
    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    // Set texture parameters for better rendering
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set unpack alignment for 1-byte bitmaps
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Initialize packing
    uint32_t currentX = 0;
    uint32_t currentY = 0;
    uint32_t maxHeightInRow = 0; // Track the tallest glyph in this specific row

    //ASCII
    for (auto charcode = FIRST_GLYPH; charcode <= LAST_GLYPH; ++charcode) {
        if (FT_Load_Char(face, charcode, FT_LOAD_RENDER)) continue;

        FT_GlyphSlot glyph = face->glyph;
        FT_Bitmap* bitmap = &glyph->bitmap;

        // If we exceed width, move to the next row
        if (currentX + bitmap->width + GLYPH_PADDING > ATLAS_WIDTH) {
            currentX = 0;
            currentY += maxHeightInRow + GLYPH_PADDING;
            maxHeightInRow = 0; // Reset for the new row
        }

        // Check atlas bounds
        if (currentY + bitmap->rows + GLYPH_PADDING > ATLAS_HEIGHT) {
            debug::log("WARNING: Atlas texture too small for font size {}\n", FONT_SIZE);
            break;
        }

        // Upload glyph to atlas
        if (bitmap->width > 0 && bitmap->rows > 0) {
            gl::TexSubImage2D(GL_TEXTURE_2D, 0, currentX, currentY, bitmap->width, bitmap->rows, GL_RED, GL_UNSIGNED_BYTE, bitmap->buffer);
        }

        // Calculate texture coordinates
        float tx_min = static_cast<float>(currentX) / ATLAS_WIDTH;
        float ty_min = static_cast<float>(currentY) / ATLAS_HEIGHT;
        float tx_max = static_cast<float>(currentX + bitmap->width) / ATLAS_WIDTH;
        float ty_max = static_cast<float>(currentY + bitmap->rows) / ATLAS_HEIGHT;

        // Store glyph
        m_Glyphs[charcode] = AtlasGlyph{
            glm::ivec2(bitmap->width, bitmap->rows),
            glm::ivec2(glyph->bitmap_left, glyph->bitmap_top),
            glm::vec2(tx_min, ty_min),
            glm::vec2(tx_max, ty_max),
            static_cast<uint32_t>(glyph->advance.x)
        };

        // Update X position and keep track of the tallest glyph in this row
        currentX += bitmap->width + GLYPH_PADDING;
        if (bitmap->rows > maxHeightInRow) {
            maxHeightInRow = bitmap->rows;
        }
    }

    // Clanup
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

auto Text::render() -> void {
    if (m_Batches.empty()) return;

    auto [width, height] = m_GApi.window().dims();
    auto projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);

    // Bind shader and set uniforms
    m_Program->use();
    m_Program->set_uniform("u_Projection", projection);
    m_Program->set_uniform("u_Color", FONT_COLOR);
    m_Program->set_uniform("u_Texture", 0);

    // Bind atlas texture
    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasTexture);

    // Bind VAO
    gl::BindVertexArray(VAO);

    // Begin batch
    m_Vertices.clear();
    m_IndexCount = 0;
    for (const auto& [pos, text] : m_Batches) {
        float startX = pos.x;
        float x = startX;
        float y = height - pos.y - static_cast<float>(m_Ascent);

        for (auto c : text) {
            auto glyph = m_Glyphs.at('?');
            try{ glyph = m_Glyphs.at(c); }catch(...){}

            float w = static_cast<float>(glyph.size.x);
            float h = static_cast<float>(glyph.size.y);

            if (x + w >= width) {
                x = startX;
                y -= FONT_SIZE;
            }

            float xpos = x + static_cast<float>(glyph.bearing.x);
            float ypos = y - static_cast<float>(glyph.size.y - glyph.bearing.y);

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

auto Text::push_quad(const glm::vec2& position, const glm::vec2& size, const glm::vec2& texMin, const glm::vec2& texMax) -> void {
    Vertex vertices[4] = {
        {glm::vec2(position.x, position.y),                 glm::vec2(texMin.x, texMax.y)},
        {glm::vec2(position.x + size.x, position.y),        glm::vec2(texMax.x, texMax.y)},
        {glm::vec2(position.x + size.x, position.y + size.y), glm::vec2(texMax.x, texMin.y)},
        {glm::vec2(position.x, position.y + size.y),        glm::vec2(texMin.x, texMin.y)}
    };

    m_Vertices.insert(m_Vertices.end(), vertices, vertices + 4);
    m_IndexCount += INDICES_PER_QUAD;
}

auto Text::text(std::string text, glm::vec2 pos) -> void {
    m_Batches[pos] = text;
}

auto Text::print(std::string text) -> void {
    static glm::vec2 currs = {0, 0};
    m_Batches[currs] = text;
    currs += glm::vec2{0, FONT_SIZE};
}