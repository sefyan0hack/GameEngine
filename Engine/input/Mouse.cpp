#include "Mouse.hpp"
#include <core/Log.hpp>
#include <core/Exception.hpp>

auto Mouse::get_pos() const noexcept -> std::pair<int32_t, int32_t>
{
	return {m_X, m_X};
}

auto Mouse::get_rawdelta() const noexcept -> std::pair<float, float>
{
    return {Sensitivity * m_Dx, Sensitivity * m_Dy};
}

auto Mouse::on_mouse_move( int32_t x, int32_t y ) noexcept -> void
{
	std::tie(m_X, m_Y) = { x, y };
}

auto Mouse::on_rawdelta( float dx, float dy ) noexcept -> void
{
	std::tie(m_Dx, m_Dy) = { dx, dy };
}

auto Mouse::on_mouse_leave() noexcept -> void
{
	isMouseIn = false;
}

auto Mouse::on_mouse_enter() noexcept -> void
{
	isMouseIn = true;
}

auto Mouse::on_button_down(Button btn) noexcept	-> void
{
	const auto index = std::to_underlying(btn);
	m_CurrButtonState[index] = true;
}

auto Mouse::on_button_up(Button btn) noexcept		-> void
{
	const auto index = std::to_underlying(btn);
	m_CurrButtonState[index] = false;
}

auto Mouse::locked() const -> bool
{
    return isLocked;
}

auto Mouse::clear_state() noexcept -> void
{
	m_CurrButtonState.reset();
	m_PrevButtonState.reset();
}

auto Mouse::save_prev_state() noexcept -> void { 
    m_PrevButtonState = m_CurrButtonState;
}

auto Mouse::is_down(Button btn) const -> bool
{
	const auto index = std::to_underlying(btn);
	return m_CurrButtonState[index];
}

auto Mouse::is_up(Button btn) const -> bool
{
    const auto index = std::to_underlying(btn);
	return !m_CurrButtonState[index];
}

auto Mouse::is_pressed(Button btn) const -> bool
{
	const auto index = std::to_underlying(btn);
	return m_CurrButtonState[index] && !m_PrevButtonState[index];;
}

auto Mouse::is_released(Button btn) const -> bool
{
    const auto index = std::to_underlying(btn);
	return !m_CurrButtonState[index] && m_PrevButtonState[index];
}