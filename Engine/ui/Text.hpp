#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

struct Glyph  {
    uint32_t TextureID;
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
    uint32_t advance;    // Offset to advance to next glyph
};

class Text {
public:
    Text(const class OpenGL& ctx);

    auto render() const -> void ;
    auto text(std::string text, glm::vec2 pos) -> void;
    auto render_single_string(const std::string& text, float x, float y, float scale) const -> void;
public:
    constexpr static const char* FONT_NAME  = "res/JetBrainsMonoNL-BoldItalic.ttf";
    constexpr static uint32_t FONT_SIZE = 25;
    constexpr static glm::vec3 FONT_COLOR = { 0.2126f, 0.7152f, 0.0722f };
private:
    const class OpenGL& m_GApi;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;
    uint32_t VAO, VBO;

    std::unordered_map<unsigned char, Glyph> m_Glyphs;
    std::unordered_map<glm::ivec2, std::string> m_Text;

    int32_t m_Ascent = 0;
    int32_t m_Descent = 0;
    int32_t m_LineGap = 0;
};
