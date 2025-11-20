#include <cstring>

#include "Window.hpp"
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Exception.hpp>
#include <core/EventQueue.hpp>

#if defined(WINDOWS_PLT)
#	include "platform/windows/Window.inl"
#elif defined(LINUX_PLT)
#	include "platform/linux/Window.inl"
#elif defined(WEB_PLT)
#	include "platform/web/Window.inl"
#endif

CWindow::CWindow(
	[[maybe_unused]] int32_t Width, [[maybe_unused]] int32_t Height, 
	[[maybe_unused]] const char* Title,
	[[maybe_unused]] EventQueue& Queue) noexcept
	: m_Width(Width)
	, m_Height(Height)
	, m_Visible(false)
	, m_FullScreen(false)
	, m_EventQueue(Queue)
{
	std::tie(m_Handle, m_Surface) = new_window(m_Width, m_Height, Title);

	#if defined(WEB_PLT)
	register_event_callbacks();
    #endif
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
    debug::print("Exit. ");
}

auto CWindow::is_fullscreen() -> bool 
{
	return m_FullScreen;
}