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

#include <ft2build.h>
#include FT_FREETYPE_H

#include <bit>
#include <vector>
#include <fstream>
#include <print>
#include <algorithm>
#include <span>

extern cmrc::embedded_filesystem embed_filesystem;

Text::Text(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_Vert(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/text.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/text.frag"), GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
{
    gl::GenVertexArrays(1, &VAO);
    gl::GenBuffers(1, &VBO);

    FT_Library ft;
    FT_Face face;

    if (FT_Init_FreeType(&ft)) {
        throw Exception("ERROR::FREETYPE: Could not init FreeType Library");
    }

    auto font_data = embed_filesystem.open(FONT_NAME);

    if (FT_New_Memory_Face(ft, std::bit_cast<FT_Byte const*>(font_data.begin()), static_cast<FT_Long>(font_data.size()), 0, &face)) {
        throw Exception("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

    m_Ascent  = face->size->metrics.ascender >> 6;
    m_Descent = face->size->metrics.descender >> 6;
    m_LineGap = (face->size->metrics.height >> 6) - m_Ascent + m_Descent;

    // FreeType bitmaps are 1-byte aligned, standard OpenGL expects 4-bytes.
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            throw Exception("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }

        GLuint texture;
        gl::GenTextures(1, &texture);
        gl::BindTexture(GL_TEXTURE_2D, texture);
        gl::TexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_Glyphs[c] = Glyph{
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
    }

    gl::BindVertexArray(VAO);
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);

    gl::BufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
}


auto Text::render() const -> void
{
    auto [width, height] = m_GApi.window().dims();

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    m_Program->use();

    m_Program->set_uniform("textColor", FONT_COLOR);
    m_Program->set_uniform("text", 0);
    m_Program->set_uniform("Projection", projection);

    gl::DepthFunc(GL_ALWAYS);   // always pass depth test
    gl::DepthMask(GL_FALSE);    // don't write depth

    for (const auto& [pos, content] : m_Text) {
        float y = height - pos.y - static_cast<float>(m_Ascent);
        render_single_string(content, pos.x, y, 1.0f);
    }

    // restore to default
    gl::DepthFunc(GL_LEQUAL);
    gl::DepthMask(GL_TRUE);
}

auto Text::text(std::string text, glm::vec2 pos) -> void
{
    m_Text[pos] = text;
}

void Text::render_single_string(const std::string& text, float x, float y, float scale) const
{
    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindVertexArray(VAO);

    for (auto c: text)
    {
        const auto& ch = m_Glyphs.at(c);

        float xpos = x + static_cast<float>(ch.bearing.x) * scale;
        float ypos = y - static_cast<float>(ch.size.y - ch.bearing.y) * scale;
        float w = static_cast<float>(ch.size.x) * scale;
        float h = static_cast<float>(ch.size.y) * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        
        gl::BindTexture(GL_TEXTURE_2D, ch.TextureID);
        
        gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        gl::BindBuffer(GL_ARRAY_BUFFER, 0);

        gl::DrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursors for next glyph (advance is number of 1/64 pixels)
        // Bitshift by 6 to get value in pixels (2^6 = 64)
        x += static_cast<float>(ch.advance >> 6) * scale; 
    }
}
