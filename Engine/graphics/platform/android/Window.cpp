#include "Window.hpp"
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>

#include <core/Event.hpp>

CWindow::~CWindow()
{
	eglTerminate(m_Display);
}

auto CWindow::android_window(void* native_window)	-> std::tuple<H_DSP, H_WIN, H_SRF>
{
	auto  window = reinterpret_cast<ANativeWindow*>(native_window);

	auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(display == EGL_NO_DISPLAY) throw Exception("EGL_NOT_INITIALIZED");

	EGLint egl_major = 0, egl_minor = 0;
    if (!eglInitialize(display, &egl_major, &egl_minor)) {
        throw Exception("eglInitialize failed");
    }

	debug::log("egl v{}.{}", egl_major, egl_minor);

	eglBindAPI(EGL_OPENGL_ES_API);

    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    auto surface = eglCreateWindowSurface(display, config, window, nullptr);

    return {display, window, surface};
}


auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
{
	throw Exception("cant create window in andoid with this :)");
}

auto CWindow::process_messages() -> void
{
}

auto CWindow::show() -> void
{
	m_Visible = true;
}
auto CWindow::hide() -> void
{
	m_Visible = false;
}

auto CWindow::toggle_fullscreen() -> void
{
	// no meaning in android
}

auto CWindow::swap_buffers() const -> void
{
    eglSwapBuffers(m_Display, m_Surface);
}

auto CWindow::get_title() -> std::string 
{
}

auto CWindow::set_title(std::string  title) -> void
{
}

auto CWindow::set_vsync(bool state) -> void
{
}

auto CWindow::message_box(const char* title, const char* body) -> bool
{
	// needs java/koltlin code  `ya333`
}


auto CWindow::dims() const	-> std::pair<int32_t, int32_t>
{
	return { ANativeWindow_getWidth(m_Handle), ANativeWindow_getHeight(m_Handle) };
}


auto CWindow::resize(int32_t width, int32_t height)	-> void
{
	// for android ?? releay
}
