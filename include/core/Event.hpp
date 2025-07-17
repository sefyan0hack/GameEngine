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

constexpr auto TotaleEvents = std::variant_size_v<Event> - 1;  // (- monostate)

template <size_t... I>
constexpr auto EventNamesImpl(std::index_sequence<I...>) 
    -> std::array<const char*, sizeof...(I)> 
{
    return { ::type_name<std::variant_alternative_t<I+1, Event>>().c_str()... };
}

constexpr auto Events() -> std::array<const char*, TotaleEvents>
{
    return EventNamesImpl(std::make_index_sequence<TotaleEvents>{});
}