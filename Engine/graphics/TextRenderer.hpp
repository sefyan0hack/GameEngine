#pragma once
#include "Renderer.hpp"
#include "Texture.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <engine_export.h>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Glyph  {
    GLuint TextureID;
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

class ENGINE_EXPORT TextRenderer
{
public:
    TextRenderer(const class OpenGL& ctx);
    
    auto render() const -> void ;
    auto text(std::string text, glm::vec2 pos) -> void;
    auto render_single_string(const std::string& text, float x, float y, float scale) const -> void;

    constexpr static uint32_t FONT_SIZE  = 25;
    constexpr static glm::vec3 FONT_COLOR = { 1, 0, 0 }; // (red)

private:
    const class OpenGL& m_GApi;
    uint32_t m_X, m_Y, m_Width, m_Height;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;
    unsigned int VAO, VBO;

    std::unordered_map<unsigned char, Glyph> m_Glyphs;
    std::unordered_map<glm::ivec2, std::string> m_Text;

    int m_Ascent = 0;
    int m_Descent = 0;
    int m_LineGap = 0;
};