#include <cstring>
#include "Window.hpp"

CWindow::CWindow(
	[[maybe_unused]] int32_t Width, [[maybe_unused]] int32_t Height, 
	[[maybe_unused]] const char* Title) noexcept
	: m_Visible(false)
	, m_FullScreen(false)
{
	std::tie(m_Display, m_Handle, m_Surface) = new_window(Width, Height, Title);
}

auto CWindow::display() const        -> H_DSP
{
	return m_Display;
}


auto CWindow::handle() const -> H_WIN
{
    return m_Handle;
}

auto CWindow::surface() const -> H_SRF
{
    return m_Surface;
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
