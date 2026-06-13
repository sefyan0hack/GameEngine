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
#include <cmath>
#include <string>
#include <vector>

Text::Text(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_Vert(std::make_shared<Shader>(res::get("res/Shaders/text.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(res::get("res/Shaders/text.frag"), GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
    , VAO(0), VBO(0)
    , m_AtlasTexture(0)
{
    // Create initial atlas
    create_atlas();
    
    // Initialize buffers
    prepare_buffers();
}

Text::~Text() {
    gl::DeleteTextures(1, &m_AtlasTexture);
    gl::DeleteVertexArrays(1, &VAO);
    gl::DeleteBuffers(1, &VBO);
}

auto Text::prepare_buffers() -> void {
    // Generate and bind VAO
    gl::GenVertexArrays(1, &VAO);
    gl::BindVertexArray(VAO);

    // --- Dynamic instance VBO (attribute 2..5) ---
    gl::GenBuffers(1, &VBO);

    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(GlyphInstance), nullptr, GL_DYNAMIC_DRAW);

    // Offset
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphInstance), (void*)offsetof(GlyphInstance, offset));
    gl::VertexAttribDivisor(0, 1);

    // Size
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphInstance), (void*)offsetof(GlyphInstance, size));
    gl::VertexAttribDivisor(1, 1);

    // TexRect
    gl::EnableVertexAttribArray(2);
    gl::VertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GlyphInstance), (void*)offsetof(GlyphInstance, texRect));
    gl::VertexAttribDivisor(2, 1);

    gl::BindVertexArray(0);
}

auto Text::create_atlas() -> void {
    auto font_data = res::get(FONT_NAME);

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, reinterpret_cast<const unsigned char*>(font_data.data()), 0)) {
        throw Exception("ERROR::STB_TRUETYPE: Failed to load font");
    }

    float scale = stbtt_ScaleForPixelHeight(&info, FONT_SIZE);

    int32_t ascent{}, descent{}, lineGap{};
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

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
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (uint32_t  i = 0; i < CHAR_COUNT; ++i) {
        const auto& bc = bakedChars[i];

        auto width = float(bc.x1 - bc.x0);
        auto height = float(bc.y1 - bc.y0);

        glyphs()[i] = AtlasGlyph {
            .offset = emath::vec2(bc.xoff, bc.yoff + (ascent * scale)), // remeber adding accennt to (y) so i dont keep it as member var
            .size = emath::vec2(width, height),
            .textRec = emath::vec4(
                bc.x0 / static_cast<float>(ATLAS_WIDTH), bc.y0 / static_cast<float>(ATLAS_HEIGHT),
                bc.x1 / static_cast<float>(ATLAS_WIDTH), bc.y1 / static_cast<float>(ATLAS_HEIGHT)
            ),
            .advance = static_cast<uint32_t>(bc.xadvance * 64.0f)
        };
    }
}

auto Text::render() -> void {
    if (m_Text.empty()) return;

    auto [width, height] = m_GApi.window().dims();
    auto projection = emath::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    std::vector<GlyphInstance> instances;
    instances.reserve(256);

    for (const auto& [pos, text] : m_Text) {
        float startX = pos.x;
        float x = startX;
        float y = static_cast<float>(height) - pos.y;

        for (auto c : text) {
            uint8_t ch = static_cast<uint8_t>(c);

            const auto& glyph =
                (ch >= FIRST_GLYPH && ch <= LAST_GLYPH)
                    ? glyphs()[ch - FIRST_GLYPH]
                    : glyphs()['?' - FIRST_GLYPH];

            float w = glyph.size.x;
            float h = glyph.size.y;

            if (x + w >= static_cast<float>(width)) {
                x = startX;
                y -= FONT_SIZE;
            }

            float xpos = x + glyph.offset.x;
            float ypos = y - h - glyph.offset.y;

            instances.push_back( GlyphInstance {
                .offset = {xpos, ypos},
                .size = {w, h},
                .texRect = glyph.textRec
            });

            x += static_cast<float>(glyph.advance >> 6);

        }
    }

    if (instances.empty()) return;

    // Upload instance data
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(GlyphInstance), instances.data());

    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);

    m_Program->use();
    m_Program->set_uniform("u_Projection", projection);
    m_Program->set_uniform("u_Color", FONT_COLOR);
    m_Program->set_uniform("u_Texture", 0);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasTexture);

    gl::BindVertexArray(VAO);

    gl::DrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<GLsizei>(instances.size()));

    gl::DepthFunc(GL_LEQUAL);
    gl::DepthMask(GL_TRUE);
}

auto Text::text(std::string text, emath::vec2 pos) -> void {
    m_Text[pos] = std::move(text);
}

auto Text::glyphs() -> std::array<AtlasGlyph, CHAR_COUNT>&
{
    static std::array<AtlasGlyph, CHAR_COUNT> _;
    return _;
}