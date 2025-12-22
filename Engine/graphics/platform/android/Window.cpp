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


void android_main(struct android_app *app)
{
	//?? how to get window to CWindow::new_window
}

auto CWindow::android_window(void* native_window)	-> std::tuple<H_DSP, H_WIN, H_SRF>
{
	auto  window = reinterpret_cast<ANativeWindow*>(native_window)

	auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(display == EGL_NOT_INITIALIZED) throw Exception("EGL_NOT_INITIALIZED");

	uint32_t egl_major = 0, egl_minor = 0;
	eglInitialize(display, &egl_major, &egl_minor);
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
    eglChooseConfig(m_Display, attribs, &config, 1, &numConfigs);

    m_Surface = eglCreateWindowSurface(m_Display, config, window, nullptr);

	m_Width = ANativeWindow_getWidth(window);
	m_Height = ANativeWindow_getHeight(window);

    return {m_Display, *window, m_Surface};
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
	debug::unimpl();
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
	debug::unimpl();
}