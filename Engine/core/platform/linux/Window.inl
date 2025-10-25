
CWindow::~CWindow()
{
	XDestroyWindow(m_Surface, m_Handle);
	if (m_Surface) {
		XCloseDisplay(m_Surface);
	}
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<H_WIN, H_SRF>
{
	auto Surface = XOpenDisplay(nullptr);

    if (!Surface) {
		throw Exception("Failed to open X display connection.");
	}
    int32_t screen = DefaultScreen(Surface);

    /* Create a window */
    auto window_handle = XCreateSimpleWindow(Surface, RootWindow(Surface, screen), 
                                 10, 10, static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1, 
                                 BlackPixel(Surface, screen), WhitePixel(Surface, screen));
    Expect(window_handle != 0, "window_handle are null ???");
    XStoreName(Surface, window_handle, Title);

	// XkbSetDetectableAutoRepeat(Surface, true, NULL);

    /* Select input events */
    XSelectInput(Surface, window_handle,
		 KeyPressMask | KeyReleaseMask | ExposureMask |
		 ResizeRedirectMask | FocusChangeMask | StructureNotifyMask
		);

	return {window_handle, Surface};
}

auto CWindow::process_messages([[maybe_unused]] CWindow* self) -> void
{
	Expect(self != nullptr, "Cwindow* self Can't be null");
	
	[[maybe_unused]] auto winHandle = self->m_Handle;
	[[maybe_unused]] auto surface = self->m_Surface;

	[[maybe_unused]] int32_t screen = DefaultScreen(surface);
	Atom wmDeleteMessage = XInternAtom(surface, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(surface, winHandle, &wmDeleteMessage, 1);

	XEvent event{};
	while (XPending(surface)) {
		XNextEvent(surface, &event);
		
		switch (event.type) {
			case Expose:
				// Handle window expose event
				break;

			case ConfigureNotify:
				// Handle window resize
				self->m_EventQueue.push(CWindow::ResizeEvent{event.xconfigure.width, event.xconfigure.height});
				break;

			case KeyPress:
			case KeyRelease: {
				const auto keycode = static_cast<KeyCode>(event.xkey.keycode);
				KeySym keysym = XkbKeycodeToKeysym(surface, keycode, 0, 0); // US layout
				
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
					self->m_EventQueue.push(Keyboard::KeyDownEvent{key});
				} else {
					self->m_EventQueue.push(Keyboard::KeyUpEvent{key});
				}
				break;
			}

			case FocusOut:
				// Clear keyboard state when window loses focus
				self->m_EventQueue.push(CWindow::LoseFocusEvent{self});
				break;

			case ClientMessage:
				if (static_cast<decltype(wmDeleteMessage)>(event.xclient.data.l[0]) == wmDeleteMessage){
					self->m_EventQueue.push(CWindow::QuitEvent{});
				}
				break;
		}
	}
}

auto CWindow::show() -> void
{
	XMapWindow(m_Surface, m_Handle);
	m_Visible = true;
}
auto CWindow::hide() -> void
{
	XUnmapWindow(m_Surface, m_Handle);
	m_Visible = false;
}

auto CWindow::toggle_fullscreen() -> void
{
	// not impl yet
}

auto CWindow::swap_buffers() const -> void
{
    ::glXSwapBuffers(m_Surface, m_Handle);
}

auto CWindow::get_title() -> std::string 
{
	std::string title;

	// brocken
	char* name = nullptr;
    if (XFetchName(m_Surface, m_Handle, &name) && name) {
		title.resize(std::strlen(name) + 1);
        XFree(name);
	}

	return title;
}

auto CWindow::set_title(std::string  title) -> void
{
    XStoreName(m_Surface, m_Handle, title.c_str());
}

auto CWindow::set_vsync(bool state) -> void
{
	if(!glXSwapIntervalEXT) glXSwapIntervalEXT = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT"));
	glXSwapIntervalEXT(m_Surface, m_Handle, state);
}
