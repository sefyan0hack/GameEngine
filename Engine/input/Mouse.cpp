#include "Mouse.hpp"

#if defined(WINDOWS_PLT)
#undef near
#undef far
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <graphics/Window.hpp>
#include <core/Log.hpp>

#if defined(WINDOWS_PLT)
#	include "platform/windows/Mouse.inl"
#elif defined(LINUX_PLT)
#	include "platform/linux/Mouse.inl"
#elif defined(WEB_PLT)
#	include "platform/web/Mouse.inl"
#endif

auto Mouse::get_pos() const noexcept -> std::pair<int32_t, int32_t>
{
	return {x, y};
}

auto Mouse::get_rawdelta() const noexcept -> std::pair<float, float>
{
    return {Sensitivity * dx, Sensitivity * dy};
}

auto Mouse::on_mouse_move( int32_t x, int32_t y ) noexcept -> void
{
	std::tie(this->x, this->y) = { x, y };
}

auto Mouse::on_rawdelta( float dx, float dy ) noexcept -> void
{
	std::tie(this->dx, this->dy) = { dx, dy };
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