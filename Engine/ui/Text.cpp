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

static auto glyphs() -> std::array<stbtt_bakedchar, Text::CHAR_COUNT>&
{
    static std::array<stbtt_bakedchar, Text::CHAR_COUNT> _;
    return _;
}

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

    // TexRect
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GlyphInstance), (void*)offsetof(GlyphInstance, texRect));
    gl::VertexAttribDivisor(1, 1);

    gl::BindVertexArray(0);
}

auto Text::create_atlas() -> void {
    auto font_data = res::get(FONT_NAME);

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, reinterpret_cast<const unsigned char*>(font_data.data()), 0)) {
        throw Exception("ERROR::STB_TRUETYPE: Failed to load font");
    }

    m_Scale = stbtt_ScaleForPixelHeight(&info, FONT_SIZE);

    stbtt_GetFontVMetrics(&info, &m_Ascent, &m_Descent, &m_LineGap);

    unsigned char bitmap[ATLAS_WIDTH * ATLAS_HEIGHT];

    int result = stbtt_BakeFontBitmap(
        reinterpret_cast<const unsigned char*>(font_data.data()), 0,
        FONT_SIZE,
        bitmap, ATLAS_WIDTH, ATLAS_HEIGHT,
        FIRST_GLYPH, CHAR_COUNT,
        glyphs().data()
    );

    if (result <= 0) {
        throw Exception("ERROR::STB_TRUETYPE: Failed to bake font bitmap");
    }

    gl::GenTextures(1, &m_AtlasTexture);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasTexture);
    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

auto Text::render() -> void {
    if (m_Text.empty()) return;

    auto [width, height] = m_GApi.window().dims();
    auto projection = emath::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    std::vector<GlyphInstance> instances;
    instances.reserve(256);

    float scaledAscent = m_Ascent * m_Scale;
    float lineHeight = (m_Ascent - m_Descent + m_LineGap) * m_Scale;

    for (const auto& [pos, text] : m_Text) {
        float startX = pos.x;
        float x = startX;
        float y = static_cast<float>(height) - pos.y - scaledAscent;

        for (auto c : text) {
            uint8_t ch = static_cast<uint8_t>(c);

            const auto& glyph =
                (ch >= FIRST_GLYPH && ch <= LAST_GLYPH)
                    ? glyphs()[ch - FIRST_GLYPH]
                    : glyphs()['?' - FIRST_GLYPH];

            float w = static_cast<float>(glyph.x1 - glyph.x0);
            float h = static_cast<float>(glyph.y1 - glyph.y0);

            if ((x + w) >= static_cast<float>(width)) {
                x = startX;
                y -= lineHeight;
            }

            float xpos = x + glyph.xoff;
            float ypos = y - glyph.yoff - h;

            instances.push_back( GlyphInstance {
                .offset = {xpos, ypos},
                .texRect = emath::vec4(
                    glyph.x0, glyph.y0,
                    glyph.x1, glyph.y1
                ),
            });

            x += glyph.xadvance;
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