#pragma once
#include <engine_export.h>

#include <emath/vec2.hpp>
#include <emath/ivec2.hpp>
#include <emath/hash.hpp>

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <array>
#include <cstdint>

#include "UiFont.hpp"

class ENGINE_EXPORT Text {
public:
    Text(const class OpenGL& ctx);
    ~Text();
    
    auto render() -> void;
    auto text(std::string text, emath::vec2 pos) -> void;    
    auto draw(std::string text) -> void;    

public:
    constexpr static const char* DEFAULT_FONT_NAME = "res/JetBrainsMonoNL-BoldItalic.ttf";
    constexpr static float DEFAULT_FONT_SIZE = 24.0f;
    inline    static emath::vec3 DEFAULT_FONT_COLOR = {139.0f, 0.0f, 0.0f};
    constexpr static size_t BATCH_SIZE = 4096;

    constexpr static uint8_t FIRST_GLYPH = 32;
    constexpr static uint8_t LAST_GLYPH = 127;
    constexpr static uint32_t GLYPH_COUNT = LAST_GLYPH - FIRST_GLYPH + 1;

private:
    auto prepare_buffers() -> void;
    auto atlas_dims() const -> std::pair<int32_t, int32_t>;

    struct Glyph {
        emath::vec2 offset;   // world position of the bottom‑left of the glyph
        emath::u16vec4 texRect;  // (uMin, vMin, uMax, vMax) in atlas
    };

private:
    const class OpenGL& m_GApi;
    std::shared_ptr<class ShaderProgram> m_Program;

    UiFont m_Font;
    emath::vec2 m_Cursor;

    uint32_t VAO, VBO, m_Atlas;

    std::unordered_map<emath::vec2, std::string> m_Text;
    std::vector<Glyph> m_Instances;
    std::array<UiFont::bakedchar, GLYPH_COUNT> m_Glyphs;
};