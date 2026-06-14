#pragma once
#include <engine_export.h>

#include <emath/vec2.hpp>
#include <emath/ivec2.hpp>
#include <emath/hash.hpp>

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <bit>

#include "Font.hpp"

class ENGINE_EXPORT Text {
public:
    Text(const class OpenGL& ctx);
    ~Text();
    
    auto render() -> void;
    auto text(std::string text, emath::vec2 pos) -> void;    
    auto draw(std::string text) -> void;    

public:
    constexpr static const char* DEFAULT_FONT_NAME = "res/JetBrainsMonoNL-BoldItalic.ttf";
    inline    static emath::vec3 FONT_COLOR = {139.0f, 0.0f, 0.0f};
    constexpr static size_t BATCH_SIZE = 4096;

private:
    auto prepare_buffers() -> void;

private:
    const class OpenGL& m_GApi;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;

    Font m_Font;
    emath::vec2 m_Cursor;

    uint32_t VAO, VBO;

    std::unordered_map<emath::vec2, std::string> m_Text;
    std::vector<Font::Glyph> m_Instances;
};