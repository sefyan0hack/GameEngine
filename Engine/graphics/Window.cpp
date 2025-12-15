#include <cstring>

#include "Window.hpp"
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Exception.hpp>
#include <core/Event.hpp>

#if defined(WINDOWS_PLT)
#	include "platform/windows/Window.inl"
#elif defined(LINUX_PLT)
#	include "platform/linux/Window.inl"
#elif defined(WEB_PLT)
#	include "platform/web/Window.inl"
#endif

CWindow::CWindow(
	[[maybe_unused]] int32_t Width, [[maybe_unused]] int32_t Height, 
	[[maybe_unused]] const char* Title) noexcept
	: m_Width(Width)
	, m_Height(Height)
	, m_Visible(false)
	, m_FullScreen(false)
{
	std::tie(m_Handle, m_Surface) = new_window(m_Width, m_Height, Title);
}

auto CWindow::handle() const -> H_WIN
{
    return m_Handle;
}

auto CWindow::surface() const -> H_SRF
{
    return m_Surface;
}

auto CWindow::width() const -> int32_t
{
    return m_Width;
}

auto CWindow::height() const -> int32_t
{
    return m_Height;
}

auto CWindow::visible() const -> bool
{
	return m_Visible;
}

auto CWindow::close() -> void
{
    hide();
}

auto CWindow::is_fullscreen() -> bool 
{
	return m_FullScreen;
}

auto CWindow::resize(int32_t width, int32_t height) -> void
{
	std::tie(m_Width, m_Height) = { width, height };
}