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

	emscripten_set_focusout_callback(m_WindowHandle, this, EM_FALSE,
		[](int32_t eventType, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
			CWindow* window = static_cast<CWindow*>(userData);
    		if (!window) return EM_TRUE;
			switch (eventType)
			{
				case EMSCRIPTEN_EVENT_BLUR:
					window->m_Events.push(LoseFocusEvent{});
					break;
			}
			return EM_TRUE;
		}
	);

	emscripten_set_wheel_callback(m_WindowHandle, this, EM_FALSE,
		[]([[maybe_unused]] int32_t eventType, const EmscriptenWheelEvent* e, void* userData) -> EM_BOOL  {
			CWindow* window = static_cast<CWindow*>(userData);
			
			window->m_Events.push(MouseWheelEvent{static_cast<int16_t>(e->deltaY * -120)});
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
			window->m_Events.push(WindowResizeEvent{ static_cast<uint16_t>(e->elementWidth), static_cast<uint16_t>(e->elementHeight)});
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
			m_Events.push(QuitEvent{});
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
        	const bool isAutoRepeat = (Lpr & (1 << 30)) != 0;  

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
					key = Keyboard::FromNative(Wpr);
			}
				
			if (key != Key::Unknown) {
				auto keyAction = isAutoRepeat ? Keyboard::Event::Type::Repeat : Keyboard::Event::Type::Press;
				m_Events.push(Keyboard::Event{key, keyAction});
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
					key = Keyboard::FromNative(Wpr);
					break;
			}
			
			if (key != Key::Unknown) {
				m_Events.push(Keyboard::Event{key, Keyboard::Event::Type::Release});
            	break;
			}
			break;
		}

	    ///////////// END KEYBOARD MESSAGES /////////////

        ///////////// MOUSE MESSAGES /////////////////
	    case WM_MOUSEMOVE:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
			m_Events.push(Mouse::Event{Mouse::Event::Type::Move, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
			return 0;
	    }
		case WM_MOUSEHOVER :{
			const POINTS pt = MAKEPOINTS( Lpr );

			m_Events.push(Mouse::Event{Mouse::Event::Type::Enter, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
			return 0;
		}
		case WM_MOUSELEAVE :{
			const POINTS pt = MAKEPOINTS( Lpr );

			m_Events.push(Mouse::Event{Mouse::Event::Type::Leave, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
			return 0;
		}
	    case WM_LBUTTONDOWN:
	    {
			const POINTS pt = MAKEPOINTS( Lpr );

	    	SetForegroundWindow( Winhandle );
			m_Events.push(Mouse::Event{Mouse::Event::Type::LPress, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
	    	break;
	    }
	    case WM_RBUTTONDOWN:
	    {
			const POINTS pt = MAKEPOINTS( Lpr );

	    	m_Events.push(Mouse::Event{Mouse::Event::Type::RPress, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
	    	break;
	    }
	    case WM_LBUTTONUP:
	    {
			const POINTS pt = MAKEPOINTS( Lpr );
			m_Events.push(Mouse::Event{Mouse::Event::Type::LRelease, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
	    	break;
	    }
	    case WM_RBUTTONUP:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );

			m_Events.push(Mouse::Event{Mouse::Event::Type::RRelease, static_cast<uint16_t>(pt.x), static_cast<uint16_t>(pt.y)});
	    	break;
			
	    }
	    case WM_MOUSEWHEEL:
	    {
	    	const auto delta = GET_WHEEL_DELTA_WPARAM( Wpr );
			
			m_Events.push(MouseWheelEvent{delta});
	    	break;
	    }
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
				m_Events.push(MouseRawEvent{static_cast<int16_t>(ri.data.mouse.lLastX), static_cast<int16_t>(ri.data.mouse.lLastY)});
	    	}
	    	break;
	    }
	    ///////////////// END RAW MOUSE MESSAGES /////////////////
        case WM_KILLFOCUS:
		m_Events.clear();
		m_Events.push(LoseFocusEvent{});
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

	XkbSetDetectableAutoRepeat(DrawContext, true, NULL);

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
    if (!window) return EM_TRUE;

    window->m_Width  = e->windowInnerWidth;
    window->m_Height = e->windowInnerHeight;

	gl::Viewport(0, 0, window->m_Width, window->m_Height);
    return EM_TRUE;
}

auto CWindow::KeyHandler(int32_t eventType, const EmscriptenKeyboardEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_TRUE;

    auto MapToVirtualKey = [](const char* code) -> uint32_t {
        // Alphanumeric keys
        if (strlen(code) == 4 && code[0] == 'K' && code[1] == 'e' && code[2] == 'y') 
            return DOM_VK_A + (code[3] - 'A'); // 'A'-'Z'
        if (strlen(code) == 6 && !strncmp(code, "Digit", 5)) 
            return DOM_VK_0 + (code[5] - '0');  // '0'-'9'

		// Function keys (F1-F24)
        if (strlen(code) > 1 && code[0] == 'F') {
            int fnum = atoi(code + 1);
            if (fnum >= 1 && fnum <= 24)
                return DOM_VK_F1 + (fnum - 1);
        }

        // Special keys
        if (strcmp(code, "Backspace") == 0) return DOM_VK_BACK_SPACE;
        if (strcmp(code, "Tab") == 0) return DOM_VK_TAB;
        if (strcmp(code, "Enter") == 0) return DOM_VK_ENTER;
        if (strcmp(code, "ShiftLeft") == 0 || strcmp(code, "ShiftRight") == 0) return DOM_VK_SHIFT;
        if (strcmp(code, "ControlLeft") == 0 || strcmp(code, "ControlRight") == 0) return DOM_VK_CONTROL;
        if (strcmp(code, "AltLeft") == 0) return DOM_VK_ALT;
		if (strcmp(code, "AltRight") == 0) return DOM_VK_ALTGR;
        if (strcmp(code, "Escape") == 0) return DOM_VK_ESCAPE;
        if (strcmp(code, "Space") == 0) return DOM_VK_SPACE;

		if (strcmp(code, "CapsLock") == 0) return DOM_VK_CAPS_LOCK;
		if (strcmp(code, "PrintScreen") == 0) return DOM_VK_PRINTSCREEN;
		if (strcmp(code, "ScrollLock") == 0) return DOM_VK_SCROLL_LOCK;
		if (strcmp(code, "Pause") == 0) return DOM_VK_PAUSE;
		
		// Navigation keys
		if (strcmp(code, "Insert") == 0) return DOM_VK_INSERT;
		if (strcmp(code, "Delete") == 0) return DOM_VK_DELETE;
		if (strcmp(code, "Home") == 0) return DOM_VK_HOME;
		if (strcmp(code, "End") == 0) return DOM_VK_END;
		if (strcmp(code, "PageUp") == 0) return DOM_VK_PAGE_UP;
		if (strcmp(code, "PageDown") == 0) return DOM_VK_PAGE_DOWN;
		
		// Arrow keys
		if (strcmp(code, "ArrowLeft") == 0) return DOM_VK_LEFT;
		if (strcmp(code, "ArrowRight") == 0) return DOM_VK_RIGHT;
		if (strcmp(code, "ArrowUp") == 0) return DOM_VK_UP;
		if (strcmp(code, "ArrowDown") == 0) return DOM_VK_DOWN;
		
		// Numpad keys
		if (strcmp(code, "Numpad0") == 0) return DOM_VK_NUMPAD0;
		if (strcmp(code, "Numpad1") == 0) return DOM_VK_NUMPAD1;
		if (strcmp(code, "Numpad2") == 0) return DOM_VK_NUMPAD2;
		if (strcmp(code, "Numpad3") == 0) return DOM_VK_NUMPAD3;
		if (strcmp(code, "Numpad4") == 0) return DOM_VK_NUMPAD4;
		if (strcmp(code, "Numpad5") == 0) return DOM_VK_NUMPAD5;
		if (strcmp(code, "Numpad6") == 0) return DOM_VK_NUMPAD6;
		if (strcmp(code, "Numpad7") == 0) return DOM_VK_NUMPAD7;
		if (strcmp(code, "Numpad8") == 0) return DOM_VK_NUMPAD8;
		if (strcmp(code, "Numpad9") == 0) return DOM_VK_NUMPAD9;
		if (strcmp(code, "NumpadDecimal") == 0) return DOM_VK_DECIMAL;
		if (strcmp(code, "NumpadDivide") == 0) return DOM_VK_DIVIDE;
		if (strcmp(code, "NumpadMultiply") == 0) return DOM_VK_MULTIPLY;
		if (strcmp(code, "NumpadSubtract") == 0) return DOM_VK_SUBTRACT;
		if (strcmp(code, "NumpadAdd") == 0) return DOM_VK_ADD;
		if (strcmp(code, "NumpadEnter") == 0) return DOM_VK_ENTER;
		if (strcmp(code, "NumpadEqual") == 0) return DOM_VK_EQUALS;
		
		// Punctuation keys
		if (strcmp(code, "Comma") == 0) return DOM_VK_COMMA;
		if (strcmp(code, "Period") == 0) return DOM_VK_PERIOD;
		if (strcmp(code, "Semicolon") == 0) return DOM_VK_SEMICOLON;
		if (strcmp(code, "Quote") == 0) return DOM_VK_QUOTE;
		if (strcmp(code, "BracketLeft") == 0) return DOM_VK_OPEN_BRACKET;
		if (strcmp(code, "BracketRight") == 0) return DOM_VK_CLOSE_BRACKET;
		if (strcmp(code, "Backslash") == 0) return DOM_VK_BACK_SLASH;
		if (strcmp(code, "Slash") == 0) return DOM_VK_SLASH;
		if (strcmp(code, "Backquote") == 0) return DOM_VK_BACK_QUOTE;
		if (strcmp(code, "Minus") == 0) return DOM_VK_HYPHEN_MINUS;
		if (strcmp(code, "Equal") == 0) return DOM_VK_EQUALS;
		
		// Modifier keys
		if (strcmp(code, "MetaLeft") == 0 || strcmp(code, "MetaRight") == 0) 
			return DOM_VK_META;

		// Media keys (optional)
		if (strcmp(code, "VolumeMute") == 0) return 0xAD;  // VK_VOLUME_MUTE
		if (strcmp(code, "VolumeDown") == 0) return 0xAE;  // VK_VOLUME_DOWN
		if (strcmp(code, "VolumeUp") == 0) return 0xAF;   // VK_VOLUME_UP
        return 0;
    };


    switch (eventType) {
        case EMSCRIPTEN_EVENT_KEYDOWN: {
            uint32_t vk = MapToVirtualKey(e->code);

            auto keyAction = e->repeat 
                ? Keyboard::Event::Type::Repeat 
                : Keyboard::Event::Type::Press;

				window->m_Events.push(Keyboard::Event{Keyboard::FromNative(vk), keyAction});
        } break;

        case EMSCRIPTEN_EVENT_KEYUP: {
            uint32_t vk = MapToVirtualKey(e->code);
			window->m_Events.push(Keyboard::Event{Keyboard::FromNative(vk), Keyboard::Event::Type::Release});
        } break;
    }

    return MapToVirtualKey(e->code) ? EM_TRUE : EM_FALSE;
}

auto CWindow::MouseHandler( int32_t eventType, const EmscriptenMouseEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_TRUE;

	Mouse::Event::Type action;
	if (e->button == 0) action = Mouse::Event::Type::LPress;
	else if (e->button == 2) action = Mouse::Event::Type::RPress;

	auto x = static_cast<uint16_t>(e->targetX); 
	auto y = static_cast<uint16_t>(e->targetY);

	switch (eventType) {

        case EMSCRIPTEN_EVENT_MOUSEDOWN:
			if (e->button == 0) action = Mouse::Event::Type::LPress;
			else if (e->button == 2) action = Mouse::Event::Type::RPress;

			window->m_Events.push(Mouse::Event{action, x, y});
            break;

        case EMSCRIPTEN_EVENT_MOUSEUP:
			if (e->button == 0) action = Mouse::Event::Type::LRelease;
			else if (e->button == 2) action = Mouse::Event::Type::RRelease;

            window->m_Events.push(Mouse::Event{action, x, y});
            break;

        case EMSCRIPTEN_EVENT_MOUSEMOVE:
			window->m_Events.push(MouseRawEvent{static_cast<int16_t>(e->movementX), static_cast<int16_t>(e->movementY)});
            window->m_Events.push(Mouse::Event{Mouse::Event::Type::Move, x, y});
            break;

        case EMSCRIPTEN_EVENT_MOUSEENTER:
            window->m_Events.push(Mouse::Event{Mouse::Event::Type::Enter, x, y});
            break;

        case EMSCRIPTEN_EVENT_MOUSELEAVE:
			window->m_Events.push(Mouse::Event{Mouse::Event::Type::Leave, x, y});
            break;
    }

    return EM_FALSE;
}

auto CWindow::TouchHandler(int32_t eventType, const EmscriptenTouchEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_TRUE;

    static std::unordered_map<int, std::pair<int16_t, int16_t>> lastPos;

    for (int i = 0; i <  e->numTouches; ++i) {
		const auto& t = e->touches[i];
        auto x = static_cast<uint16_t>(t.targetX);
        auto y = static_cast<uint16_t>(t.targetY);
		
		Mouse::Event::Type action;

        switch (eventType) {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
				lastPos[id] = { x, y };
                action = Mouse::Event::Type::LPress;
                break;
            case EMSCRIPTEN_EVENT_TOUCHEND:
            case EMSCRIPTEN_EVENT_TOUCHCANCEL:
				lastPos.erase(id);
                action = Mouse::Event::Type::LRelease;
                break;
            case EMSCRIPTEN_EVENT_TOUCHMOVE:{
			 	// lookup previous
				auto old = lastPos.find(id);
				if (old != lastPos.end()) {
					int16_t dx = x - old->second.first;
					int16_t dy = y - old->second.second;
					// push a raw‐delta event
					window->m_Events.push(MouseRawEvent{ dx, dy });
				}
				// also update stored pos
				lastPos[id] = { x, y };
				action = Mouse::Event::Type::Move;
				}
                break;
        }
		window->m_Events.push(Mouse::Event{action, x, y});
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
				self->m_Events.push(WindowResizeEvent{static_cast<uint16_t>(event.xconfigure.width), static_cast<uint16_t>(event.xconfigure.height)});
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
					self->m_Events.push(Keyboard::Event{Keyboard::FromNative(vk), Keyboard::Event::Type::Press});
				} else {
					self->m_Events.push(Keyboard::Event{Keyboard::FromNative(vk), Keyboard::Event::Type::Release});
				}
				break;
				break;
			}

			case FocusOut:
				// Clear keyboard state when window loses focus
				self->m_Events.push(LoseFocusEvent{});
				break;

			case ClientMessage:
				if (event.xclient.data.l[0] == wmDeleteMessage){
					self->m_Events.push(QuitEvent{});
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
