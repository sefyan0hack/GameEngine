#include "Keyboard.hpp"

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