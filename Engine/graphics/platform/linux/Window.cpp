#include "Window.hpp"
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>

#include <core/Event.hpp>
#include <X11/Xlib.h>

CWindow::CWindow([[maybe_unused]] void* state) noexcept
{
	throw Exception("this Ctor Expected to run only on android for now");
}

CWindow::~CWindow()
{
	XDestroyWindow(m_Display, m_Handle);
	if (m_Display) {
		XCloseDisplay(m_Display);
	}
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
{
	auto Display = XOpenDisplay(nullptr);

    if (!Display) {
		throw Exception("Failed to open X display connection.");
	}
    int32_t screen = DefaultScreen(Display);

    /* Create a window */
    auto window_handle = XCreateSimpleWindow(Display, RootWindow(Display, screen), 
                                 10, 10, static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1, 
                                 BlackPixel(Display, screen), WhitePixel(Display, screen));
    Expect(window_handle != 0, "window_handle are null ???");
    XStoreName(Display, window_handle, Title);

	// XkbSetDetectableAutoRepeat(Display, true, NULL);

    /* Select input events */
    XSelectInput(Display, window_handle,
		KeyPressMask | KeyReleaseMask | ExposureMask |
		ResizeRedirectMask | FocusChangeMask | StructureNotifyMask | ButtonPress |
        ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
		PointerMotionMask | Button1MotionMask | VisibilityChangeMask
		);

	return {Display, window_handle, window_handle};
}

auto CWindow::process_messages() -> void
{
	[[maybe_unused]] int32_t screen = DefaultScreen(m_Display);
	Atom wmDeleteMessage = XInternAtom(m_Display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(m_Display, m_Handle, &wmDeleteMessage, 1);

	XEvent event{};
	while (XPending(m_Display)) {
		XNextEvent(m_Display, &event);
		
		switch (event.type) {
			case Expose:
				// Handle window expose event
				break;

			case ConfigureNotify:
				// Handle window resize
				EventQ::self().push(CWindow::ResizeEvent{event.xconfigure.width, event.xconfigure.height});
				break;

			case KeyPress:
			case KeyRelease: {
				const auto keycode = static_cast<KeyCode>(event.xkey.keycode);
				KeySym keysym = XkbKeycodeToKeysym(m_Display, keycode, 0, 0); // US layout
				
				uint32_t vk = 0;
				
				if (keysym >= XK_A && keysym <= XK_Z) {
					vk = static_cast<uint32_t>('A' + (keysym - XK_A));
				}
				else if (keysym >= XK_a && keysym <= XK_z) {
					vk = static_cast<uint32_t>('A' + (keysym - XK_a));
				}
				else if (keysym >= XK_0 && keysym <= XK_9) {
					vk = static_cast<uint32_t>('0' + (keysym - XK_0));
				}
				else {
					vk = static_cast<uint32_t>(keysym);
				}

				Key key = Keyboard::from_native(vk);

				if (event.type == KeyPress) {
					EventQ::self().push(Keyboard::KeyDownEvent{key});
				} else {
					EventQ::self().push(Keyboard::KeyUpEvent{key});
				}
				break;
			}

			case FocusOut:
				// Clear keyboard state when window loses focus
				// EventQ::self().push(CWindow::LoseFocusEvent{self}); // TODO: how to get window ref here?
				break;

			case ClientMessage:
				if (static_cast<decltype(wmDeleteMessage)>(event.xclient.data.l[0]) == wmDeleteMessage){
					EventQ::self().push(CWindow::QuitEvent{});
				}
				break;
			case MotionNotify:
				{
				EventQ::self().push(Mouse::MoveEvent{event.xmotion.x, event.xmotion.y});
				// in fut  `static variables like this will cause issues with multiple windows`
				static int prevX = event.xmotion.x;
				static int prevY = event.xmotion.y;
				
				int deltaX = event.xmotion.x - prevX;
				int deltaY = event.xmotion.y - prevY;
				
				EventQ::self().push(Mouse::MovementEvent{
					static_cast<float>(deltaX), 
					static_cast<float>(deltaY)
				});
				
				prevX = event.xmotion.x;
				prevY = event.xmotion.y;
				}
				break;
			case ButtonPress:
				switch(event.xbutton.button) {
					case Button1:
						EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Left});
						break;
					case Button2:
						EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Middle});
						break;
					case Button3:
						EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Right});
						break;
					case Button4:
						EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::WheelUp});
						break;
					case Button5:
						EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::WheelDown});
						break;
					default:
						break;
				}
				break;
			case ButtonRelease:
				switch(event.xbutton.button) {
					case Button1:
						EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::Left});
						break;
					case Button2:
						EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::Middle});
						break;
					case Button3:
						EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::Right});
						break;
					case Button4:
						EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::WheelUp});
						break;
					case Button5:
						EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::WheelDown});
						break;
					default:
						break;
				}
				break;
            case EnterNotify:
				EventQ::self().push(Mouse::EnterEvent{});
				break;
			case LeaveNotify:
				EventQ::self().push(Mouse::LeaveEvent{});
				break;
		}
	}
}

auto CWindow::show() -> void
{
	XMapWindow(m_Display, m_Handle);
	m_Visible = true;
}
auto CWindow::hide() -> void
{
	XUnmapWindow(m_Display, m_Handle);
	m_Visible = false;
}

auto CWindow::toggle_fullscreen() -> void
{
	// x11 ??
}

auto CWindow::swap_buffers() const -> void
{
    ::glXSwapBuffers(m_Display, m_Handle);
}

auto CWindow::get_title() -> std::string 
{
	std::string title;

	// brocken
	char* name = nullptr;
    if (XFetchName(m_Display, m_Handle, &name) && name) {
		title.resize(std::strlen(name) + 1);
        XFree(name);
	}

	return title;
}

auto CWindow::set_title(std::string  title) -> void
{
    XStoreName(m_Display, m_Handle, title.c_str());
}

auto CWindow::set_vsync(bool state) -> void
{
	if(!glXSwapIntervalEXT) glXSwapIntervalEXT = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT"));
	glXSwapIntervalEXT(m_Display, m_Handle, state);
}

auto CWindow::message_box(const char* title, const char* body) -> bool
{
	return true;
	// TODO: use x11
}

auto CWindow::dims() const	-> std::pair<int32_t, int32_t>
{
	XWindowAttributes attrb;
	XGetWindowAttributes(m_Display, m_Handle, &attrb);
	return { attrb.width, attrb.height };
}


auto CWindow::resize(int32_t width, int32_t height)	-> void
{
	XResizeWindow(
		m_Display,
		m_Handle,
		width,
		height
	);

	XFlush(m_Display);
}

auto CWindow::android_window(void*) -> std::tuple<H_DSP, H_WIN, H_SRF>
{
	throw Exception("Not for {}", os::host::name());
}