#pragma once
#include <variant>
#include <type_traits>
#include <utility>

#include "Utils.hpp"

using Event = std::variant<
    std::monostate,
    struct Keyboard::KeyDownEvent,
    struct Keyboard::KeyUpEvent,
    struct Mouse::ButtonDownEvent,
    struct Mouse::ButtonUpEvent,
    struct Mouse::MoveEvent,
    struct Mouse::EnterEvent,
    struct Mouse::LeaveEvent,
    struct Mouse::MovementEvent,
    struct CWindow::ResizeEvent,
    struct CWindow::LoseFocusEvent,
    struct CWindow::SetFocusEvent,
    struct CWindow::QuitEvent
>;

constexpr auto Events = utils::variant_to_array<Event>();

template <typename T>
concept EventType = std::is_same_v<T, std::monostate> || ::type_name<T>().ends_with("Event");

template <typename T>
concept ISEvent = Variant<T> && [](){
    using V = std::remove_cvref_t<T>;
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (EventType<std::variant_alternative_t<I, V>> && ...);
    }(std::make_index_sequence<std::variant_size_v<V>>{});
}();
