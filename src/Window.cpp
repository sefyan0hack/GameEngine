#include <core/Window.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Event.hpp>


CWindow::CWindow([[maybe_unused]] int32_t Width, [[maybe_unused]] int32_t Height, [[maybe_unused]] const char* Title, [[maybe_unused]] bool withopengl) 
	: m_Width(Width)
	, m_Height(Height)
	, m_Visible(false)
	, m_refCount(1)
	, m_Events()
{
	std::tie(m_WindowHandle, m_DrawContext) = new_window(m_Width, m_Height, Title);

	S_WindowsCount++;

	if(withopengl) m_OpenGl = std::make_shared<gl::OpenGL>(m_WindowHandle, m_DrawContext);

	#if defined(WEB_PLT)

	emscripten_set_keypress_callback(m_WindowHandle, this, EM_FALSE, &CWindow::KeyHandler);
	emscripten_set_keydown_callback(m_WindowHandle, this, EM_FALSE, &CWindow::KeyHandler);
	emscripten_set_keyup_callback(m_WindowHandle, this, EM_FALSE, &CWindow::KeyHandler);
	
	emscripten_set_mousedown_callback(m_WindowHandle , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mouseup_callback(m_WindowHandle    , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mousemove_callback(m_WindowHandle  , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mouseenter_callback(m_WindowHandle , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mouseleave_callback(m_WindowHandle , this, EM_FALSE, &CWindow::MouseHandler);

	emscripten_set_touchstart_callback(m_WindowHandle, this, EM_FALSE, &CWindow::TouchHandler);
	emscripten_set_touchmove_callback(m_WindowHandle, this, EM_FALSE, &CWindow::TouchHandler);
	emscripten_set_touchend_callback(m_WindowHandle, this, EM_FALSE, &CWindow::TouchHandler);
	emscripten_set_touchcancel_callback(m_WindowHandle, this, EM_FALSE, &CWindow::TouchHandler);

	emscripten_set_focus_callback(m_WindowHandle, this, EM_FALSE,
		[](int32_t eventType, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
			CWindow* window = static_cast<CWindow*>(userData);
    		if (!window) return EM_FALSE;

			switch (eventType)
			{
				case EMSCRIPTEN_EVENT_FOCUS:
					window->m_Events.push(WindowFocusEvent{true});
					break;
				case EMSCRIPTEN_EVENT_BLUR:
					window->m_Events.push(WindowFocusEvent{false});
					break;
			}
			return EM_TRUE;
		}
	);

	emscripten_set_fullscreenchange_callback(m_WindowHandle, this, EM_FALSE, 
		[](
			int32_t eventType, 
			const EmscriptenFullscreenChangeEvent* e,
			void* userData
		) -> EM_BOOL {
			auto* window = static_cast<CWindow*>(userData);

			if (e->isFullscreen) Info("Enable FullScreen");
			window->m_Events.push(WindowResizeEvent{ e->elementWidth, e->elementHeight});
			return EM_TRUE;
	});
	
    #endif
}

CWindow::CWindow(const CWindow& other)
	: m_WindowHandle(other.m_WindowHandle)
	, m_DrawContext(other.m_DrawContext)
	, m_Width(other.m_Width)
	, m_Height(other.m_Height)
	, m_Visible(other.m_Visible)
	, m_RawBuffer(other.m_RawBuffer)
	, m_OpenGl(other.m_OpenGl)
	, m_refCount(other.m_refCount)
	, m_Events(other.m_Events)
{
	m_refCount++;
}

CWindow::~CWindow()
{
	m_refCount--;
	if (m_refCount == 0) {
		m_OpenGl.reset();
		#if defined(WINDOWS_PLT)
		DestroyWindow(m_WindowHandle);
		#elif defined(LINUX_PLT)
		XDestroyWindow(m_DrawContext, m_WindowHandle);
		if (m_DrawContext) {
			XCloseDisplay(m_DrawContext);
		}
		#elif defined(WEB_PLT)
		if (m_OpenGl->Context()) {
			emscripten_webgl_destroy_context(m_OpenGl->Context());
		}
		#endif
	}
}

#if defined(WINDOWS_PLT)

// CWindow class things///////////////////////////////////
CWindow::WinClass &CWindow::WinClass::Instance()
{
	static CWindow::WinClass ClassIns; 
    return ClassIns;
}
auto CWindow::WinClass::Name() -> const TCHAR*
{
	return m_Name;
}
CWindow::WinClass::WinClass(){
	
	m_WinclassEx.cbSize = sizeof(WNDCLASSEX);
    m_WinclassEx.style =  CS_HREDRAW | CS_VREDRAW;
    m_WinclassEx.lpfnWndProc = CWindow::WinProcThunk;
    m_WinclassEx.hInstance =  GetModuleHandle(nullptr);
    m_WinclassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_WinclassEx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    m_WinclassEx.lpszClassName = m_Name;
		
	m_Windclass = RegisterClassEx(&m_WinclassEx);
		
    if(m_Windclass == 0){
        Error("faild to regester class {}", GetLastError());
    }
}

///////////////////////////////////////////////////////////////////

auto CWindow::WinProcThunk(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    if (msg == WM_NCCREATE){
        const auto WinptrStruct = reinterpret_cast<CREATESTRUCTW*>(Lpr);
        auto const pWindow  = reinterpret_cast<CWindow*>(WinptrStruct->lpCreateParams);
        SetWindowLongPtrA(Winhandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

		return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
    }

	// Route messages to instance handler
    if (auto pWindow = reinterpret_cast<CWindow*>(GetWindowLongPtrA(Winhandle, GWLP_USERDATA))) {
        return pWindow->WinProcFun(Winhandle, msg, Wpr, Lpr);
    }

    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CALLBACK CWindow::WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    switch (msg)
    {
        case WM_CREATE:{
			Info("Creat Main CWindow");
            return 0;
        }
        case WM_CLOSE:{
			m_Events.push(WindowQuitEvent{});
			return 0;
        }
        case WM_SIZE:{
			m_Events.push(WindowResizeEvent{LOWORD(Lpr), HIWORD(Lpr)});
            return 0;
        }
        /*********** KEYBOARD MESSAGES ***********/
	    case WM_KEYDOWN:
	    case WM_SYSKEYDOWN: {

			const bool isExtended   = (Lpr & (1 << 24)) != 0;

			Key key = Key::Unknown;
				
			switch (Wpr) {
				case VK_SHIFT: {
					const UINT scancode = (Lpr & 0x00FF0000) >> 16;
					key = (MapVirtualKeyA(scancode, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT) 
						   ? Key::RightShift : Key::LeftShift;
					break;
				}
				case VK_CONTROL:
					key = isExtended ? Key::RightControl : Key::LeftControl;
					break;
				case VK_MENU:
					key = isExtended ? Key::RightAlt : Key::LeftAlt;
					break;
				default:
					key = Keyboard::FromNative(static_cast<uint32_t>(Wpr));
			}
				
			if (key != Key::Unknown) {
				m_Events.push(Keyboard::KeyDownEvent{key});
			}
		}
		break;
		
		case WM_KEYUP:
		case WM_SYSKEYUP: {
			const bool isExtended = (Lpr & (1 << 24)) != 0;
			Key key = Key::Unknown;

			switch (Wpr) {
				case VK_SHIFT: {
					const UINT scancode = (Lpr & 0x00FF0000) >> 16;
					key = (MapVirtualKeyA(scancode, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT) 
						   ? Key::RightShift : Key::LeftShift;
					break;
				}
				case VK_CONTROL:
					key = isExtended ? Key::RightControl : Key::LeftControl;
					break;
				case VK_MENU:
					key = isExtended ? Key::RightAlt : Key::LeftAlt;
					break;
				default:
					key = Keyboard::FromNative(static_cast<uint32_t>(Wpr));
					break;
			}
			
			if (key != Key::Unknown) {
				m_Events.push(Keyboard::KeyUpEvent{key});
            	break;
			}
			break;
		}

	    ///////////// END KEYBOARD MESSAGES /////////////

        ///////////// MOUSE MESSAGES /////////////////
	    case WM_MOUSEMOVE:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
			m_Events.push(Mouse::MoveEvent{pt.x, pt.y});
			return 0;
	    }
		case WM_MOUSEHOVER :{
			m_Events.push(Mouse::EnterEvent{});
			return 0;
		}
		case WM_MOUSELEAVE :{
			m_Events.push(Mouse::LeaveEvent{});
			return 0;
		}
	    case WM_LBUTTONDOWN:
			m_Events.push(Mouse::ButtonDownEvent{Mouse::Button::Left});
			break;
		case WM_MBUTTONDOWN:
			m_Events.push(Mouse::ButtonDownEvent{Mouse::Button::Middle});
			break;
	    case WM_RBUTTONDOWN:
			m_Events.push(Mouse::ButtonDownEvent{Mouse::Button::Right});
			break;
		case WM_LBUTTONUP:
			m_Events.push(Mouse::ButtonUpEvent{Mouse::Button::Left});
			break;
		case WM_MBUTTONUP:
			m_Events.push(Mouse::ButtonUpEvent{Mouse::Button::Middle});
			break;
	    case WM_RBUTTONUP:
	    	m_Events.push(Mouse::ButtonUpEvent{Mouse::Button::Right});
			break;
	    ///////////////// END MOUSE MESSAGES /////////////////

	    ///////////////// RAW MOUSE MESSAGES /////////////////
	    case WM_INPUT:
	    {
	    	UINT size{};
	    	// first get the size of the input data
	    	if( GetRawInputData( reinterpret_cast<HRAWINPUT>(Lpr), RID_INPUT, nullptr, &size, sizeof( RAWINPUTHEADER ) ) == (UINT)-1)
	    	{
	    		//my log error hire
	    		break;
	    	}
	    	m_RawBuffer.resize( size );
	    	// read in the input data
	    	if( GetRawInputData(
	    		reinterpret_cast<HRAWINPUT>(Lpr),
	    		RID_INPUT,
	    		m_RawBuffer.data(),
	    		&size,
	    		sizeof( RAWINPUTHEADER ) ) != size )
	    	{
				//my log error hire
	    		break;
	    	}
	    	// process the raw input data
	    	auto& ri = reinterpret_cast<const RAWINPUT&>(*m_RawBuffer.data());
	    	if( ri.header.dwType == RIM_TYPEMOUSE &&
	    		(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0) )
				{
				m_Events.push(Mouse::RawDeltaEvent{ri.data.mouse.lLastX, ri.data.mouse.lLastY});
	    	}
	    	break;
	    }
	    ///////////////// END RAW MOUSE MESSAGES /////////////////
        case WM_KILLFOCUS:
		m_Events.clear();
		m_Events.push(WindowFocusEvent{});
		ClipCursor(nullptr); //release cursor confinement
		break;

    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<WindHandl, HDC_D>
{
    WinClass::Instance();

    auto window_handle = CreateWindow(
        MAKEINTATOM(WinClass::m_Windclass),
        Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
        nullptr, nullptr,
        GetModuleHandleA( nullptr ),
        this
    );

    if(window_handle == nullptr){
        Error("faild to creat CWindow code : {}", GetLastError());
    }

	return {window_handle, GetDC(window_handle)};
}

#elif defined(LINUX_PLT)

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<WindHandl, HDC_D>
{
	auto DrawContext = XOpenDisplay(nullptr);

    if (!DrawContext) {
		Error("Failed to open X display connection.");
	}
    int32_t screen = DefaultScreen(DrawContext);

    /* Create a window */
    auto window_handle = XCreateSimpleWindow(DrawContext, RootWindow(DrawContext, screen), 
                                 10, 10, static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1, 
                                 BlackPixel(DrawContext, screen), WhitePixel(DrawContext, screen));
    Expect(window_handle != 0, "window_handle are null ???");
    XStoreName(DrawContext, window_handle, Title);

	// XkbSetDetectableAutoRepeat(DrawContext, true, NULL);

    /* Select input events */
    XSelectInput(DrawContext, window_handle,
		 KeyPressMask | KeyReleaseMask | ExposureMask |
		 ResizeRedirectMask | FocusChangeMask | StructureNotifyMask
		);

	return {window_handle, DrawContext};
}

#elif defined(WEB_PLT)
auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<WindHandl, HDC_D>
{
	auto window_handle = "#canvas";
	void* DrawContext = nullptr;

	emscripten_set_window_title(Title);
	emscripten_set_canvas_element_size(window_handle, Width, Height);

	return {window_handle, DrawContext};
}

auto CWindow::ResizeHandler(int32_t eventType, const EmscriptenUiEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;
	if(eventType == EMSCRIPTEN_EVENT_RESIZE){
		window->m_Events.push(WindowResizeEvent{e->windowInnerWidth, e->windowInnerHeight});
		return EM_TRUE;
	}

	return EM_FALSE;
}

auto CWindow::KeyHandler(int32_t eventType, const EmscriptenKeyboardEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;
	constexpr auto MAX_UINT32_T = std::numeric_limits<uint32_t>::max();

    uint32_t vk = MAX_UINT32_T;

    // Alphanumeric keys
	if (strncmp(e->code, "Key", 3) == 0 && e->code[3] >= 'A' && e->code[3] <= 'Z') {
        vk = DOM_VK_A + (e->code[3] - 'A');
    }
	else if (strncmp(e->code, "Digit", 5) == 0 && e->code[5] >= '0' && e->code[5] <= '9') {
        vk = DOM_VK_0 + (e->code[5] - '0');
    }

	// Function keys (F1-F24)
    else if (strlen(e->code) > 1 && e->code[0] == 'F') {
        int fnum = atoi(e->code + 1);
        if (fnum >= 1 && fnum <= 24)
            vk = DOM_VK_F1 + (fnum - 1);
    }

    // Special keys
    else if (strcmp(e->code, "Backspace") == 0) vk = DOM_VK_BACK_SPACE;
    else if (strcmp(e->code, "Tab") == 0) vk = DOM_VK_TAB;
    else if (strcmp(e->code, "Enter") == 0) vk = DOM_VK_ENTER;
    else if (strcmp(e->code, "ShiftLeft") == 0 || strcmp(e->code, "ShiftRight") == 0) vk = DOM_VK_SHIFT;
    else if (strcmp(e->code, "ControlLeft") == 0 || strcmp(e->code, "ControlRight") == 0) vk = DOM_VK_CONTROL;
    else if (strcmp(e->code, "AltLeft") == 0) vk = DOM_VK_ALT;
	else if (strcmp(e->code, "AltRight") == 0) vk = DOM_VK_ALTGR;
    else if (strcmp(e->code, "Escape") == 0) vk = DOM_VK_ESCAPE;
    else if (strcmp(e->code, "Space") == 0) vk = DOM_VK_SPACE;

	else if (strcmp(e->code, "CapsLock") == 0) vk = DOM_VK_CAPS_LOCK;
	else if (strcmp(e->code, "PrintScreen") == 0) vk = DOM_VK_PRINTSCREEN;
	else if (strcmp(e->code, "ScrollLock") == 0) vk = DOM_VK_SCROLL_LOCK;
	else if (strcmp(e->code, "Pause") == 0) vk = DOM_VK_PAUSE;

	// Navigation keys
	else if (strcmp(e->code, "Insert") == 0) vk = DOM_VK_INSERT;
	else if (strcmp(e->code, "Delete") == 0) vk = DOM_VK_DELETE;
	else if (strcmp(e->code, "Home") == 0) vk = DOM_VK_HOME;
	else if (strcmp(e->code, "End") == 0) vk = DOM_VK_END;
	else if (strcmp(e->code, "PageUp") == 0) vk = DOM_VK_PAGE_UP;
	else if (strcmp(e->code, "PageDown") == 0) vk = DOM_VK_PAGE_DOWN;

	// Arrow keys
	else if (strcmp(e->code, "ArrowLeft") == 0) vk = DOM_VK_LEFT;
	else if (strcmp(e->code, "ArrowRight") == 0) vk = DOM_VK_RIGHT;
	else if (strcmp(e->code, "ArrowUp") == 0) vk = DOM_VK_UP;
	else if (strcmp(e->code, "ArrowDown") == 0) vk = DOM_VK_DOWN;

	// Numpad keys
	else if (strcmp(e->code, "Numpad0") == 0) vk = DOM_VK_NUMPAD0;
	else if (strcmp(e->code, "Numpad1") == 0) vk = DOM_VK_NUMPAD1;
	else if (strcmp(e->code, "Numpad2") == 0) vk = DOM_VK_NUMPAD2;
	else if (strcmp(e->code, "Numpad3") == 0) vk = DOM_VK_NUMPAD3;
	else if (strcmp(e->code, "Numpad4") == 0) vk = DOM_VK_NUMPAD4;
	else if (strcmp(e->code, "Numpad5") == 0) vk = DOM_VK_NUMPAD5;
	else if (strcmp(e->code, "Numpad6") == 0) vk = DOM_VK_NUMPAD6;
	else if (strcmp(e->code, "Numpad7") == 0) vk = DOM_VK_NUMPAD7;
	else if (strcmp(e->code, "Numpad8") == 0) vk = DOM_VK_NUMPAD8;
	else if (strcmp(e->code, "Numpad9") == 0) vk = DOM_VK_NUMPAD9;
	else if (strcmp(e->code, "NumpadDecimal") == 0) vk = DOM_VK_DECIMAL;
	else if (strcmp(e->code, "NumpadDivide") == 0) vk = DOM_VK_DIVIDE;
	else if (strcmp(e->code, "NumpadMultiply") == 0) vk = DOM_VK_MULTIPLY;
	else if (strcmp(e->code, "NumpadSubtract") == 0) vk = DOM_VK_SUBTRACT;
	else if (strcmp(e->code, "NumpadAdd") == 0) vk = DOM_VK_ADD;
	else if (strcmp(e->code, "NumpadEnter") == 0) vk = DOM_VK_ENTER;
	else if (strcmp(e->code, "NumpadEqual") == 0) vk = DOM_VK_EQUALS;

	// Punctuation keys
	else if (strcmp(e->code, "Comma") == 0) vk = DOM_VK_COMMA;
	else if (strcmp(e->code, "Period") == 0) vk = DOM_VK_PERIOD;
	else if (strcmp(e->code, "Semicolon") == 0) vk = DOM_VK_SEMICOLON;
	else if (strcmp(e->code, "Quote") == 0) vk = DOM_VK_QUOTE;
	else if (strcmp(e->code, "BracketLeft") == 0) vk = DOM_VK_OPEN_BRACKET;
	else if (strcmp(e->code, "BracketRight") == 0) vk = DOM_VK_CLOSE_BRACKET;
	else if (strcmp(e->code, "Backslash") == 0) vk = DOM_VK_BACK_SLASH;
	else if (strcmp(e->code, "Slash") == 0) vk = DOM_VK_SLASH;
	else if (strcmp(e->code, "Backquote") == 0) vk = DOM_VK_BACK_QUOTE;
	else if (strcmp(e->code, "Minus") == 0) vk = DOM_VK_HYPHEN_MINUS;
	else if (strcmp(e->code, "Equal") == 0) vk = DOM_VK_EQUALS;

	// Modifier keys
	else if (strcmp(e->code, "MetaLeft") == 0 || strcmp(e->code, "MetaRight") == 0) 
		vk = DOM_VK_META;

	// Browser control keys (DOM Level 3)
	else if (strcmp(e->code, "BrowserBack") == 0) vk = DOM_VK_BROWSER_BACK;
	else if (strcmp(e->code, "BrowserForward") == 0) vk = DOM_VK_BROWSER_FORWARD;
	else if (strcmp(e->code, "BrowserRefresh") == 0) vk = DOM_VK_BROWSER_REFRESH;
	else if (strcmp(e->code, "BrowserStop") == 0) vk = DOM_VK_BROWSER_STOP;
	else if (strcmp(e->code, "BrowserSearch") == 0) vk = DOM_VK_BROWSER_SEARCH;
	else if (strcmp(e->code, "BrowserFavorites") == 0) vk = DOM_VK_BROWSER_FAVORITES;
	else if (strcmp(e->code, "BrowserHome") == 0) vk = DOM_VK_BROWSER_HOME;

	// Media control keys (DOM Level 3)
	else if (strcmp(e->code, "VolumeMute") == 0) vk = DOM_VK_VOLUME_MUTE;
	else if (strcmp(e->code, "VolumeDown") == 0) vk = DOM_VK_VOLUME_DOWN;
	else if (strcmp(e->code, "VolumeUp") == 0) vk =  DOM_VK_VOLUME_UP;
	else if (strcmp(e->code, "MediaTrackNext") == 0) vk = DOM_VK_MEDIA_TRACK_NEXT;
	else if (strcmp(e->code, "MediaTrackPrevious") == 0) vk = DOM_VK_MEDIA_TRACK_PREVIOUS;
	else if (strcmp(e->code, "MediaStop") == 0) vk = DOM_VK_MEDIA_STOP;
	else if (strcmp(e->code, "MediaPlayPause") == 0) vk = DOM_VK_MEDIA_PLAY_PAUSE;

	if (vk != MAX_UINT32_T) {
        Key key = Keyboard::FromNative(vk);

		switch (eventType)
		{
			case EMSCRIPTEN_EVENT_KEYDOWN:
				window->m_Events.push(Keyboard::KeyDownEvent{key});
				break;
			case EMSCRIPTEN_EVENT_KEYUP:
				window->m_Events.push(Keyboard::KeyUpEvent{key});
				break;
		}
        return EM_TRUE;
    }

	return EM_FALSE;
}

auto CWindow::MouseHandler( int32_t eventType, const EmscriptenMouseEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;

	auto btn = 
		e->button == 0 ? Mouse::Button::Left:
		e->button == 1 ? Mouse::Button::Middle: Mouse::Button::Right;

	switch (eventType) {

        case EMSCRIPTEN_EVENT_MOUSEDOWN:
			window->m_Events.push(Mouse::ButtonDownEvent{btn});
			break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
			window->m_Events.push(Mouse::ButtonUpEvent{btn});
            break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
			window->m_Events.push(Mouse::RawDeltaEvent{e->movementX, e->movementY});
            window->m_Events.push(Mouse::MoveEvent{e->targetX, e->targetY});
            break;

        case EMSCRIPTEN_EVENT_MOUSEENTER:
            window->m_Events.push(Mouse::EnterEvent{});
            break;

        case EMSCRIPTEN_EVENT_MOUSELEAVE:
			window->m_Events.push(Mouse::LeaveEvent{});
            break;
    }

    return EM_TRUE;
}

auto CWindow::TouchHandler(int32_t eventType, const EmscriptenTouchEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;

    static std::unordered_map<int32_t, std::pair<int16_t, int16_t>> lastPos;
	// Check if in mobile mode using media query
	bool isMobile = (bool) EM_ASM_INT({
        return window.matchMedia('(max-width: 767px)').matches ? 1 : 0;
    });

	auto screenwidth = EM_ASM_DOUBLE({return window.screen.width;});
	auto screenheight = EM_ASM_DOUBLE({return window.screen.height;});

	double canvaswidth = 0.0;
    double canvasheight = 0.0;

    emscripten_get_element_css_size(window->Handle(), &canvaswidth, &canvasheight);

    for (int32_t i = 0; i <  e->numTouches; ++i) {
		const auto& t = e->touches[i];
		if (!t.isChanged) 
			continue;

		int32_t id = t.identifier;

		double x = static_cast<double>(t.targetX);
        double y = static_cast<double>(t.targetY);

        // 1. Normalize coordinates to screen dimensions
        x *= (screenwidth / canvaswidth);
        y *= (screenheight / canvasheight);

		if (isMobile) {
            // Rotate coordinates 90 degrees counter-clockwise
            double newX = screenheight - y;  // Inverted y becomes x
            double newY = x;                 // x becomes y
            x = newX;
            y = newY;
            
            // Swap width/height references for mobile
            std::swap(screenwidth, screenheight);
        }

        switch (eventType) {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
				lastPos[id] = { static_cast<int16_t>(x), static_cast<int16_t>(y) };
				window->m_Events.push(Mouse::EnterEvent{});
                break;
				case EMSCRIPTEN_EVENT_TOUCHEND:
				case EMSCRIPTEN_EVENT_TOUCHCANCEL:
				lastPos.erase(id);
				window->m_Events.push(Mouse::LeaveEvent{});
                break;
				case EMSCRIPTEN_EVENT_TOUCHMOVE:{
			 	// lookup previous
				auto old = lastPos.find(id);
				if (old != lastPos.end()) {
					int32_t dx = static_cast<int32_t>(x) - old->second.first;
					int32_t dy = static_cast<int32_t>(y) - old->second.second;
					// push a raw‐delta event
					window->m_Events.push(Mouse::RawDeltaEvent{ dx, dy });
				}
				// also update stored pos
				lastPos[id] = { x, y };
				window->m_Events.push(Mouse::MoveEvent{static_cast<int32_t>(x), static_cast<int32_t>(y)});
			}
            break;
        }
    }

    return EM_FALSE;
}
#endif

auto CWindow::ProcessMessages([[maybe_unused]] CWindow* self) -> void
{
	Expect(self != nullptr, "Cwindow* self Can't be null");
	
	[[maybe_unused]] auto winHandle = self->m_WindowHandle;
	[[maybe_unused]] auto DrawCtx = self->m_DrawContext;

	#if defined(WINDOWS_PLT)
    MSG Msg = {};
    while (PeekMessageA(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
	#elif defined(LINUX_PLT)
	int32_t screen = DefaultScreen(DrawCtx);
	Atom wmDeleteMessage = XInternAtom(DrawCtx, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(DrawCtx, winHandle, &wmDeleteMessage, 1);

	XEvent event{};
	while (XPending(DrawCtx)) {
		XNextEvent(DrawCtx, &event);
		
		switch (event.type) {
			case Expose:
				// Handle window expose event
				break;

			case ConfigureNotify:
				// Handle window resize
				self->m_Events.push(WindowResizeEvent{event.xconfigure.width, event.xconfigure.height});
				break;

			case KeyPress:
			case KeyRelease: {
				const KeyCode keycode = event.xkey.keycode;
				KeySym keysym = XkbKeycodeToKeysym(DrawCtx, keycode, 0, 0); // US layout
				
				// Handle alphanumeric keys separately
				uint32_t vk = 0;
				
				// Letters (A-Z) - normalize to uppercase
				if (keysym >= XK_A && keysym <= XK_Z) {
					vk = 'A' + (keysym - XK_A);
				}
				// Letters (a-z) - normalize to uppercase
				else if (keysym >= XK_a && keysym <= XK_z) {
					vk = 'A' + (keysym - XK_a);
				}
				// Numbers (0-9)
				else if (keysym >= XK_0 && keysym <= XK_9) {
					vk = '0' + (keysym - XK_0);
				}
				// Other keys - use keysym directly
				else {
					vk = static_cast<uint32_t>(keysym);
				}

				if (event.type == KeyPress) {
					self->m_Events.push(Keyboard::KeyDownEvent{Keyboard::FromNative(vk)});
				} else {
					self->m_Events.push(Keyboard::KeyUpEvent{Keyboard::FromNative(vk)});
				}
				break;
			}

			case FocusOut:
				// Clear keyboard state when window loses focus
				self->m_Events.push(WindowFocusEvent{});
				break;

			case ClientMessage:
				if (event.xclient.data.l[0] == wmDeleteMessage){
					self->m_Events.push(WindowQuitEvent{});
				}
				break;
		}
	}
	#elif defined(WEB_PLT)
	#endif
}

auto CWindow::Handle() const -> WindHandl
{
    return m_WindowHandle;
}

auto CWindow::DrawContext() const -> HDC_D
{
    return m_DrawContext;
}

auto CWindow::Width() const -> int32_t
{
    return m_Width;
}

auto CWindow::Height() const -> int32_t
{
    return m_Height;
}
auto CWindow::opengl() const -> std::shared_ptr<gl::OpenGL>
{
	return m_OpenGl;
}
auto CWindow::Visible() const -> bool
{
	return m_Visible;
}
auto CWindow::WindowsCount() -> unsigned short
{
	return S_WindowsCount;
}
auto CWindow::Show() -> void
{
	#if defined(WINDOWS_PLT)
	ShowWindow(m_WindowHandle, SW_SHOW);
	#elif defined(LINUX_PLT)
	XMapWindow(m_DrawContext, m_WindowHandle);
	#endif
	m_Visible = true;
}
auto CWindow::Hide() -> void
{
	#if defined(WINDOWS_PLT)
	ShowWindow(m_WindowHandle, SW_HIDE);
	#elif defined(LINUX_PLT)
	XUnmapWindow(m_DrawContext, m_WindowHandle);
	#endif
	m_Visible = false;
}

bool CWindow::PollEvent(Event& event) { return m_Events.poll(event); }
void CWindow::WaitEvent(Event& event) { m_Events.wait_and_poll(event); }
void CWindow::ClearEvents() { m_Events.clear(); }

auto CWindow::WindowShouldClose() -> bool
{
	#if !defined(WEB_PLT)
	return S_WindowsCount == 0;
	#else
	emscripten_sleep(12);
	return false;
	#endif
}

auto CWindow::ToggleFullScreen() -> void
{
	#if defined(WINDOWS_PLT)
	struct FullscreenData {
        RECT restoreRect;
        LONG_PTR style;
        LONG_PTR exStyle;
        bool wasMaximized;
    };
    const char* propName = "FullscreenData";
    FullscreenData* data = static_cast<FullscreenData*>(GetProp(m_WindowHandle, propName));
    
    if (!data) {
        // ENTER FULLSCREEN
        data = new FullscreenData();
        
        // Save current state
        data->style = GetWindowLongPtr(m_WindowHandle, GWL_STYLE);
        data->exStyle = GetWindowLongPtr(m_WindowHandle, GWL_EXSTYLE);
        data->wasMaximized = !!::IsZoomed(m_WindowHandle);
        GetWindowRect(m_WindowHandle, &data->restoreRect);
        
        // Get monitor info
        HMONITOR hmon = MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
		mi.cbSize = sizeof(mi);
        GetMonitorInfo(hmon, &mi);
        
        // Set window to cover entire monitor WITHOUT changing style
        SetWindowPos(m_WindowHandle, HWND_TOP,
            mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER
        );
        
        // Optional: Remove window decorations if desired
        SetWindowLongPtr(m_WindowHandle, GWL_STYLE, WS_VISIBLE | WS_POPUP);
        
        SetProp(m_WindowHandle, propName, data);
    } else {
        // EXIT FULLSCREEN
        // Restore original style
        SetWindowLongPtr(m_WindowHandle, GWL_STYLE, data->style);
        SetWindowLongPtr(m_WindowHandle, GWL_EXSTYLE, data->exStyle);
        
        // Restore window position
        SetWindowPos(m_WindowHandle, nullptr,
            data->restoreRect.left,
            data->restoreRect.top,
            data->restoreRect.right - data->restoreRect.left,
            data->restoreRect.bottom - data->restoreRect.top,
            SWP_NOZORDER | SWP_FRAMECHANGED
        );
        
        // Restore maximized state if needed
        if (data->wasMaximized) {
            ShowWindow(m_WindowHandle, SW_MAXIMIZE);
        }
        
        RemoveProp(m_WindowHandle, propName);
        delete data;
    }
    
    // Update window immediately
    UpdateWindow(m_WindowHandle);
	#elif defined(LINUX_PLT)
	#elif defined(WEB_PLT)

	EmscriptenFullscreenChangeEvent fullscrendata;
    EMSCRIPTEN_RESULT ret = emscripten_get_fullscreen_status(&fullscrendata);

	if (!fullscrendata.isFullscreen) {
		ret = emscripten_request_fullscreen("#canvas", 1);
	} else {
		ret = emscripten_exit_fullscreen();
	}

	#endif
}

auto CWindow::SwapBuffers() const -> void
{
	#if defined(WINDOWS_PLT)
    ::SwapBuffers(m_DrawContext);
    #elif defined(LINUX_PLT)
    ::glXSwapBuffers(m_DrawContext, m_WindowHandle);
    #endif
}

auto CWindow::Close() -> void
{
    Hide();
	--S_WindowsCount;
    Info("Exit. ");
}
