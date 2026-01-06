#include "Window.hpp"
#include "gl.hpp"

#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <engine_export.h>

#include <android/log.h>

android_app* g_android_app = nullptr;

auto input_callback(android_app* state, AInputEvent* event) -> int32_t
{
    auto type = AInputEvent_getType(event);

    if (type == AINPUT_EVENT_TYPE_MOTION) {
        int32_t actionWithIndex = AMotionEvent_getAction(event);
        int32_t action = actionWithIndex & AMOTION_EVENT_ACTION_MASK;
        
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);

        switch (action) {
            case AMOTION_EVENT_ACTION_DOWN:
                // Update position and click
                EventQ::self().push(Mouse::MoveEvent{ (int)x, (int)y });
                EventQ::self().push(Mouse::ButtonDownEvent{ Mouse::Button::Left });
                return 1;

            case AMOTION_EVENT_ACTION_UP:
                EventQ::self().push(Mouse::ButtonUpEvent{ Mouse::Button::Left });
                return 1;

            case AMOTION_EVENT_ACTION_MOVE:
                EventQ::self().push(Mouse::MoveEvent{ (int)x, (int)y });
                return 1;
                
            case AMOTION_EVENT_ACTION_CANCEL:
                // prevent "stuck" buttons if a gesture is interrupted
                EventQ::self().push(Mouse::ButtonUpEvent{ Mouse::Button::Left });
                return 1;
        }
    }
    else if (type == AINPUT_EVENT_TYPE_KEY) {
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        int32_t action = AKeyEvent_getAction(event);
        
        // Skip repeat events  EventQ doesn't need them
        if (AKeyEvent_getRepeatCount(event) > 0) return 1;

        Key k = Keyboard::from_native(keyCode);
        
        if (action == AKEY_EVENT_ACTION_DOWN) {
            EventQ::self().push(Keyboard::KeyDownEvent{k});
        } else if (action == AKEY_EVENT_ACTION_UP) {
            EventQ::self().push(Keyboard::KeyUpEvent{k});
        }
        
        // Return 1 consume the key (preventing system 'Back' etc.)
        // Return 0 the OS handle it (standard for Vol Up/Down)
        return (keyCode == AKEYCODE_BACK) ? 1 : 0;
    }

    return 0;
}

extern int main(int argc, char** argv);
void android_main(android_app* app)
{
    g_android_app = app;
    app->onInputEvent = input_callback;

    while (g_android_app->window == nullptr && !g_android_app->destroyRequested) {
        int events;
        struct android_poll_source* source;
        if (ALooper_pollOnce(-1, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(g_android_app, source);
        }
    }

    debug::logger = [](std::string msg){
        __android_log_print(ANDROID_LOG_INFO, "ENGINE", msg.c_str());
    };

    static char* args[2] = {"android", "\0"};
    main(1, args);
}


auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
{
    (void)Width;
    (void)Height;
    (void)Title;
	auto window = g_android_app->window;

    if (window == nullptr) {
        throw Exception("Cannot create surface: ANativeWindow is null");
    }

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
    if (!eglChooseConfig(display, visualAttribs, &config, 1, &numConfigs) || numConfigs < 1) {
        throw Exception("No EGL config found. Check gl::ChannelBits settings.");
    }

    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    auto surface = eglCreateWindowSurface(display, config, window, nullptr);

    if (surface == EGL_NO_SURFACE) {
        throw Exception("Failed to create surface (Error: {})", eglGetError());
    }

    return {display, window, surface};
}

CWindow::~CWindow()
{
    eglDestroySurface(m_Display, m_Surface);
	eglTerminate(m_Display);
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
    return "";
}

auto CWindow::set_title(std::string title) -> void
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
