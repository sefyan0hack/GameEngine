#pragma once

#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>
#include <core/Utils.hpp>

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

constexpr auto Events()
{
    return utils::Variant_to_Array<Event>();
}

// constexpr auto Events() -> std::array<TypeInfo, TotaleEvents>
// {
//     return []<std::size_t... I>(std::index_sequence<I...>) {
//         return std::array<TypeInfo, sizeof...(I)> {{
//             { ::type_hash<std::variant_alternative_t<I + 1, Event>>(),
//               ::type_name<std::variant_alternative_t<I + 1, Event>>()
//             }...
//         }};
//     }(std::make_index_sequence<TotaleEvents>{});
// }