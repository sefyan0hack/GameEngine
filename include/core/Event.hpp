#pragma once

class CWindow;
class Keyboard;
class Mouse;

using Event = std::variant<
    std::monostate,
    Keyboard::KeyDownEvent,
    Keyboard::KeyUpEvent,
    Mouse::ButtonDownEvent,
    Mouse::ButtonUpEvent,
    Mouse::MoveEvent,
    Mouse::EnterEvent,
    Mouse::LeaveEvent,
    Mouse::MovementEvent,
    CWindow::ResizeEvent,
    CWindow::LoseFocusEvent,
    CWindow::SetFocusEvent,
    CWindow::QuitEvent
>;

constexpr auto Events()
{
    return utils::Variant_to_Array<Event>();
}
