#pragma once

class Keyboard;
class Mouse;

struct WindowResizeEvent { int32_t width, height; };
struct WindowFocusEvent { bool focus; };
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
    WindowFocusEvent,
    WindowQuitEvent
>;