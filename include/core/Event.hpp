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

constexpr auto Events = utils::Variant_to_Array<Event>();

template <typename T>
concept EventType = std::is_same_v<T, std::monostate> || ::type_name<T>().ends_with("Event");

template <typename T>
concept ISEvent = Variant<T> && [](){
    using V = std::remove_cvref_t<T>;
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (EventType<std::variant_alternative_t<I, V>> && ...);
    }(std::make_index_sequence<std::variant_size_v<V>>{});
}();
