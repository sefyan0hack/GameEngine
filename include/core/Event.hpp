#pragma once

#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>

struct WindowResizeEvent { int32_t width, height; };
struct WindowLoseFocusEvent { const CWindow* window; };
struct WindowSetFocusEvent  { const CWindow* window; };
struct WindowQuitEvent {};

using Event = std::variant<
    std::monostate,
    Keyboard::KeyDownEvent,
    Keyboard::KeyUpEvent,
    Mouse::ButtonDownEvent,
    Mouse::ButtonUpEvent,
    Mouse::MoveEvent,
    Mouse::EnterEvent,
    Mouse::LeaveEvent,
    Mouse::RawDeltaEvent,
    WindowResizeEvent,
    WindowLoseFocusEvent,
    WindowSetFocusEvent,
    WindowQuitEvent
>;