#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <emath/vec2.hpp>
#include <emath/vec4.hpp>

#include "Text.hpp"
#include "../../res/shaders/defines.h"

struct UiShape
{
    int32_t shape;
    emath::vec4 position_size;
    emath::vec4 color;
    emath::vec4 params;
};

class Ui 
{
    Ui(const struct Keyboard& Keyboard, const struct Mouse& Mouse) : Keyboard(Keyboard), Mouse(Mouse) {}

    auto text(emath::vec2 pos, std::string text) -> void
    {
        Text.text(text, pos);
    }

    auto button(emath::vec2 pos, emath::vec2 size, emath::vec4 color, std::string text) -> void
    {
        auto textWidth = Text.text_width(text);
        auto textHeight = Text.text_height(text);

        size.x = std::max(size.x, textWidth + 20.0f);
        size.y = std::max(size.y, textHeight + 10.0f);

        emath::vec2 textPos{
            pos.x + (size.x - textWidth) * 0.5f,
            pos.y + (size.y - textHeight) * 0.5f
        };

        Ui::text(textPos, text);
    
        m_Shapes.push_back( UiShape {
            .shape = GLSL::SHAPE_rounded_rectangle,
            .position_size = {pos.x, pos.y, size.x, size.y},
            .color = color,
            .params = {10.0f, 0, 0, 0}
        });
    }

private:
    const struct Keyboard& Keyboard;
    const struct Mouse& Mouse;
    Text Text;
    std::vector<UiShape> m_Shapes;
};