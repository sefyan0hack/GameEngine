#include "Keyboard.hpp"
#include <core/Log.hpp>

#include <Keyboard.inl>

auto Keyboard::is_down(Key key) const noexcept -> bool
{
    return m_CurrKeyState[std::to_underlying(key)];
}
auto Keyboard::is_up(Key key) const noexcept -> bool
{
    return !m_CurrKeyState[std::to_underlying(key)];
}

auto Keyboard::is_pressed(Key key) const noexcept -> bool
{
    const auto index = std::to_underlying(key);
    return m_CurrKeyState[index] && !m_PrevKeyState[index];
}

auto Keyboard::is_released(Key key) const noexcept -> bool
{
    const auto index = std::to_underlying(key);
    return !m_CurrKeyState[index] && m_PrevKeyState[index];
}

auto Keyboard::on_key_down(Key key) noexcept -> void
{
    const auto index = std::to_underlying(key);
    m_CurrKeyState[index] = true;
}

auto Keyboard::on_key_up(Key key) noexcept -> void
{
    const auto index = std::to_underlying(key);
    m_CurrKeyState[index] = false;
}

auto Keyboard::save_prev_state() noexcept -> void { 
    m_PrevKeyState = m_CurrKeyState;
}

auto Keyboard::clear_state() noexcept -> void
{
	m_CurrKeyState.reset();
	m_PrevKeyState.reset();
}

/////////////////////////////////////////////////////////

auto Keyboard::from_native(uint32_t key) -> Key
{
    #define X(KEY, NATIVE_KEY) if(key == NATIVE_KEY) return Key::KEY;
    KEY_NATIVE_MAP
    return Key::Unknown;
    #undef X
}

auto Keyboard::to_native(Key key) -> uint32_t
{
    #define X(KEY, NATIVE_KEY) if(key == Key::KEY) return NATIVE_KEY;
    KEY_NATIVE_MAP
    debug::unrechable();
    #undef X
}