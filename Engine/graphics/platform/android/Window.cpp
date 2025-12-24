#include "Window.hpp"
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>

#include <core/Event.hpp>



auto input_callback(struct android_app* state, AInputEvent* event) -> int32_t
{
    auto type = AInputEvent_getType(event);
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        int32_t action = AMotionEvent_getAction(event);

        if (action == AMOTION_EVENT_ACTION_DOWN) {
            EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Left});
        } else if (action == AMOTION_EVENT_ACTION_MOVE) {
            EventQ::self().push(Mouse::MoveEvent{ (int)x, (int)y });
        }
        return 1;
    }
    else if (type == AINPUT_EVENT_TYPE_KEY) {
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        int32_t action = AKeyEvent_getAction(event);
        
        Key k = Keyboard::from_native(keyCode);
        if (action == AKEY_EVENT_ACTION_DOWN) {
            EventQ::self().push(Keyboard::KeyDownEvent{k});
        } else {
            EventQ::self().push(Keyboard::KeyUpEvent{k});
        }
    }
    return 0;
}

CWindow::CWindow(void* state) noexcept
{
    auto app = reinterpret_cast<android_app*>(state);
    app->onInputEvent = input_callback;
	std::tie(m_Display, m_Handle, m_Surface) = android_window(app);
}

CWindow::~CWindow()
{
    eglDestroySurface(m_Display, m_Surface);
	eglTerminate(m_Display);
}

auto CWindow::android_window(void* state)	-> std::tuple<H_DSP, H_WIN, H_SRF>
{
    auto app = reinterpret_cast<android_app*>(state);
	auto window = app->window;

	auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(display == EGL_NO_DISPLAY) throw Exception("EGL_NOT_INITIALIZED");

	EGLint egl_major = 0, egl_minor = 0;
    if (!eglInitialize(display, &egl_major, &egl_minor)) {
        throw Exception("eglInitialize failed");
    }

    eglBindAPI(EGL_OPENGL_ES_API);
	debug::log("egl v{}.{}", egl_major, egl_minor);

    static const EGLint visualAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, gl::ChannelBits,
        EGL_GREEN_SIZE, gl::ChannelBits,
        EGL_RED_SIZE, gl::ChannelBits,
        EGL_ALPHA_SIZE, gl::AlphaBits,
        EGL_DEPTH_SIZE, gl::DepthBufferBits, // maybe 16 ??
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, visualAttribs, &config, 1, &numConfigs);

    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    auto surface = eglCreateWindowSurface(display, config, window, nullptr);

    if (surface == EGL_NO_SURFACE) {
        throw Exception("Failed to create surface (Error: {})", eglGetError());
    }

    return {display, window, surface};
}


auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
{
	throw Exception("cant create window in android with this :)");
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
