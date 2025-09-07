#include <cstring>

#include "Window.hpp"
#include "Log.hpp"
#include "OpenGL.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "EventQueue.hpp"

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


CWindow::~CWindow()
{
	#if defined(WINDOWS_PLT)
	DestroyWindow(m_Handle);
	#elif defined(LINUX_PLT)
	XDestroyWindow(m_Surface, m_Handle);
	if (m_Surface) {
		XCloseDisplay(m_Surface);
	}
	#endif
}

#if defined(WINDOWS_PLT)

// CWindow class things///////////////////////////////////
CWindow::WinClass &CWindow::WinClass::instance()
{
	static CWindow::WinClass ClassIns; 
    return ClassIns;
}
auto CWindow::WinClass::name() -> const TCHAR*
{
	return m_Name;
}
CWindow::WinClass::WinClass(){
	
	m_WinclassEx.cbSize = sizeof(WNDCLASSEX);
    m_WinclassEx.style =  CS_HREDRAW | CS_VREDRAW;
    m_WinclassEx.lpfnWndProc = CWindow::win_proc_thunk;
    m_WinclassEx.hInstance =  GetModuleHandle(nullptr);
    m_WinclassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_WinclassEx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    m_WinclassEx.lpszClassName = m_Name;
		
	m_Windclass = RegisterClassEx(&m_WinclassEx);
		
    if(m_Windclass == 0){
        throw CException("faild to regester class {}", GetLastError());
    }
}

///////////////////////////////////////////////////////////////////

auto CWindow::win_proc_thunk(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    if (msg == WM_NCCREATE){
        const auto WinptrStruct = reinterpret_cast<CREATESTRUCTW*>(Lpr);
        auto const pWindow  = reinterpret_cast<CWindow*>(WinptrStruct->lpCreateParams);
        SetWindowLongPtrA(Winhandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

		return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
    }

	// Route messages to instance handler
    if (auto pWindow = reinterpret_cast<CWindow*>(GetWindowLongPtrA(Winhandle, GWLP_USERDATA))) {
        return pWindow->win_proc_fun(Winhandle, msg, Wpr, Lpr);
    }

    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CALLBACK CWindow::win_proc_fun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    switch (msg)
    {
        case WM_CREATE:{
			debug::print("Creat Main CWindow");
            return 0;
        }
        case WM_CLOSE:{
			m_EventQueue.push(CWindow::QuitEvent{});
			return 0;
        }
        case WM_SIZE:{
			m_EventQueue.push(CWindow::ResizeEvent{LOWORD(Lpr), HIWORD(Lpr)});
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
					key = Keyboard::from_native(static_cast<uint32_t>(Wpr));
			}
				
			if (key != Key::Unknown) {
				m_EventQueue.push(Keyboard::KeyDownEvent{key});
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
					key = Keyboard::from_native(static_cast<uint32_t>(Wpr));
					break;
			}
			
			if (key != Key::Unknown) {
				m_EventQueue.push(Keyboard::KeyUpEvent{key});
            	break;
			}
			break;
		}

	    ///////////// END KEYBOARD MESSAGES /////////////

        ///////////// MOUSE MESSAGES /////////////////
	    case WM_MOUSEMOVE:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
			m_EventQueue.push(Mouse::MoveEvent{pt.x, pt.y});
			return 0;
	    }
		case WM_MOUSEHOVER :{
			m_EventQueue.push(Mouse::EnterEvent{});
			return 0;
		}
		case WM_MOUSELEAVE :{
			m_EventQueue.push(Mouse::LeaveEvent{});
			return 0;
		}
	    case WM_LBUTTONDOWN:
			m_EventQueue.push(Mouse::ButtonDownEvent{Mouse::Button::Left});
			break;
		case WM_MBUTTONDOWN:
			m_EventQueue.push(Mouse::ButtonDownEvent{Mouse::Button::Middle});
			break;
	    case WM_RBUTTONDOWN:
			m_EventQueue.push(Mouse::ButtonDownEvent{Mouse::Button::Right});
			break;
		case WM_LBUTTONUP:
			m_EventQueue.push(Mouse::ButtonUpEvent{Mouse::Button::Left});
			break;
		case WM_MBUTTONUP:
			m_EventQueue.push(Mouse::ButtonUpEvent{Mouse::Button::Middle});
			break;
	    case WM_RBUTTONUP:
	    	m_EventQueue.push(Mouse::ButtonUpEvent{Mouse::Button::Right});
			break;
	    ///////////////// END MOUSE MESSAGES /////////////////

	    ///////////////// RAW MOUSE MESSAGES /////////////////
	    case WM_INPUT:
	    {
			static std::vector<std::byte> RawBuffer;
	    	UINT size{};
	    	// first get the size of the input data
	    	if( GetRawInputData( reinterpret_cast<HRAWINPUT>(Lpr), RID_INPUT, nullptr, &size, sizeof( RAWINPUTHEADER ) ) == (UINT)-1)
	    	{
	    		//my log error hire
	    		break;
	    	}
	    	RawBuffer.resize( size );
	    	// read in the input data
	    	if( GetRawInputData(
	    		reinterpret_cast<HRAWINPUT>(Lpr),
	    		RID_INPUT,
	    		RawBuffer.data(),
	    		&size,
	    		sizeof( RAWINPUTHEADER ) ) != size )
	    	{
				//my log error hire
	    		break;
	    	}
	    	// process the raw input data
	    	auto& ri = reinterpret_cast<const RAWINPUT&>(*RawBuffer.data());
	    	if( ri.header.dwType == RIM_TYPEMOUSE &&
	    		(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0) )
				{
				m_EventQueue.push(Mouse::MovementEvent{
					static_cast<float>(ri.data.mouse.lLastX),
					static_cast<float>(ri.data.mouse.lLastY)
				});
	    	}
	    	break;
	    }
	    ///////////////// END RAW MOUSE MESSAGES /////////////////
		case WM_SETFOCUS:
			m_EventQueue.push(CWindow::SetFocusEvent{this});
			break;
        case WM_KILLFOCUS:
			m_EventQueue.push(CWindow::LoseFocusEvent{this});
			ClipCursor(nullptr); //release cursor confinement
			break;

    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<H_WIN, H_SRF>
{
    WinClass::instance();

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
        throw CException("faild to creat CWindow code : {}", GetLastError());
    }

	return {window_handle, GetDC(window_handle)};
}

#elif defined(LINUX_PLT)

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<H_WIN, H_SRF>
{
	auto Surface = XOpenDisplay(nullptr);

    if (!Surface) {
		throw CException("Failed to open X display connection.");
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

#elif defined(WEB_PLT)
auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<H_WIN, H_SRF>
{
	auto window_handle = EMSCRIPTEN_EVENT_TARGET_WINDOW;
	auto Surface = "#canvas";

	emscripten_set_window_title(Title);
	emscripten_set_canvas_element_size(Surface, Width, Height);
	EM_ASM({
        var canvas = document.getElementById('canvas');
        if (!canvas) canvas = document.querySelector('#canvas');
        
        canvas.tabIndex = 0;
        
        canvas.addEventListener('click', function() {
            canvas.focus();
        });
        
        canvas.addEventListener('touchstart', function() {
            canvas.focus();
        });
        
        canvas.focus();
	});

	return {window_handle, Surface};
}

auto CWindow::resize_callback(int32_t eventType, const EmscriptenUiEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;
	if(eventType == EMSCRIPTEN_EVENT_RESIZE){
		window->m_EventQueue.push(CWindow::ResizeEvent{e->windowInnerWidth, e->windowInnerHeight});
	}

	return EM_TRUE;
}

auto CWindow::keyboard_callback(int32_t eventType, const EmscriptenKeyboardEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;
    constexpr auto INVALID = std::numeric_limits<uint32_t>::max();

    uint32_t vk = INVALID;

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

	if (vk != INVALID) {
        Key key = Keyboard::from_native(vk);

		switch (eventType)
		{
			case EMSCRIPTEN_EVENT_KEYDOWN:
				window->m_EventQueue.push(Keyboard::KeyDownEvent{key});
				break;
			case EMSCRIPTEN_EVENT_KEYUP:
				window->m_EventQueue.push(Keyboard::KeyUpEvent{key});
				break;
		}
    }

	return EM_TRUE;
}

auto CWindow::mouse_callback( int32_t eventType, const EmscriptenMouseEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_FALSE;

	auto btn = 
		e->button == 0 ? Mouse::Button::Left:
		e->button == 1 ? Mouse::Button::Middle: Mouse::Button::Right;

	switch (eventType) {

        case EMSCRIPTEN_EVENT_MOUSEDOWN:
			window->m_EventQueue.push(Mouse::ButtonDownEvent{btn});
			break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
			window->m_EventQueue.push(Mouse::ButtonUpEvent{btn});
            break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
			window->m_EventQueue.push(Mouse::MoveEvent{e->targetX, e->targetY});
			window->m_EventQueue.push(Mouse::MovementEvent{static_cast<float>(e->movementX), static_cast<float>(e->movementY)});
            break;

        case EMSCRIPTEN_EVENT_MOUSEENTER:
            window->m_EventQueue.push(Mouse::EnterEvent{});
            break;

        case EMSCRIPTEN_EVENT_MOUSELEAVE:
			window->m_EventQueue.push(Mouse::LeaveEvent{});
            break;
    }

    return EM_TRUE;
}

auto CWindow::touch_callback(int32_t eventType, const EmscriptenTouchEvent* e, void* userData) -> EM_BOOL
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

    emscripten_get_element_css_size(window->Surface(), &canvaswidth, &canvasheight);

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
				window->m_EventQueue.push(Mouse::EnterEvent{});
                break;
				case EMSCRIPTEN_EVENT_TOUCHEND:
				case EMSCRIPTEN_EVENT_TOUCHCANCEL:
				lastPos.erase(id);
				window->m_EventQueue.push(Mouse::LeaveEvent{});
                break;
				case EMSCRIPTEN_EVENT_TOUCHMOVE:{
			 	// lookup previous
				auto old = lastPos.find(id);
				if (old != lastPos.end()) {
					int32_t dx = static_cast<int32_t>(x) - old->second.first;
					int32_t dy = static_cast<int32_t>(y) - old->second.second;
					// push a raw‐delta event
					window->m_EventQueue.push(Mouse::MovementEvent{ static_cast<float>(dx), static_cast<float>(dy) });
				}
				// also update stored pos
				lastPos[id] = { x, y };
				window->m_EventQueue.push(Mouse::MoveEvent{static_cast<int32_t>(x), static_cast<int32_t>(y)});
			}
            break;
        }
    }

    return EM_TRUE;
}

auto CWindow::register_event_callbacks() -> void
{
	emscripten_set_keypress_callback(m_Surface, this, EM_FALSE, &CWindow::KeyboardCallback);
	emscripten_set_keydown_callback(m_Surface, this, EM_FALSE, &CWindow::KeyboardCallback);
	emscripten_set_keyup_callback(m_Surface, this, EM_FALSE, &CWindow::KeyboardCallback);

	emscripten_set_mousedown_callback(m_Surface , this, EM_FALSE, &CWindow::MouseCallback);
	emscripten_set_mouseup_callback(m_Surface    , this, EM_FALSE, &CWindow::MouseCallback);
	emscripten_set_mousemove_callback(m_Surface  , this, EM_FALSE, &CWindow::MouseCallback);
	emscripten_set_mouseenter_callback(m_Surface , this, EM_FALSE, &CWindow::MouseCallback);
	emscripten_set_mouseleave_callback(m_Surface , this, EM_FALSE, &CWindow::MouseCallback);

	emscripten_set_touchstart_callback(m_Surface, this, EM_FALSE, &CWindow::TouchCallback);
	emscripten_set_touchmove_callback(m_Surface, this, EM_FALSE, &CWindow::TouchCallback);
	emscripten_set_touchend_callback(m_Surface, this, EM_FALSE, &CWindow::TouchCallback);
	emscripten_set_touchcancel_callback(m_Surface, this, EM_FALSE, &CWindow::TouchCallback);

	emscripten_set_focus_callback(m_Surface, this, EM_FALSE,
		[](int32_t, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
			CWindow* window = static_cast<CWindow*>(userData);
    		if (!window) return EM_FALSE;

			window->m_EventQueue.push(CWindow::SetFocusEvent{window});
			return EM_TRUE;
		}
	);

	emscripten_set_blur_callback(m_Surface, this, EM_FALSE,
		[](int32_t, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
			CWindow* window = static_cast<CWindow*>(userData);
    		if (!window) return EM_FALSE;

			window->m_EventQueue.push(CWindow::LoseFocusEvent{window});
			return EM_TRUE;
		}
	);

	emscripten_set_fullscreenchange_callback(m_Surface, this, EM_FALSE, 
		[](
			int32_t eventType, 
			const EmscriptenFullscreenChangeEvent* e,
			void* userData
		) -> EM_BOOL {
			auto* window = static_cast<CWindow*>(userData);

			if (e->isFullscreen) debug::print("Enable FullScreen");
			window->m_EventQueue.push(CWindow::ResizeEvent{ e->elementWidth, e->elementHeight});
			return EM_TRUE;
	});
}
#endif

auto CWindow::process_messages([[maybe_unused]] CWindow* self) -> void
{
	Expect(self != nullptr, "Cwindow* self Can't be null");
	
	[[maybe_unused]] auto winHandle = self->m_Handle;
	[[maybe_unused]] auto surface = self->m_Surface;

	#if defined(WINDOWS_PLT)
    MSG Msg = {};
    while (PeekMessageA(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
	#elif defined(LINUX_PLT)
	int32_t screen = DefaultScreen(surface);
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
				const KeyCode keycode = event.xkey.keycode;
				KeySym keysym = XkbKeycodeToKeysym(surface, keycode, 0, 0); // US layout
				
				uint32_t vk = 0;
				
				if (keysym >= XK_A && keysym <= XK_Z) {
					vk = 'A' + (keysym - XK_A);
				}
				else if (keysym >= XK_a && keysym <= XK_z) {
					vk = 'A' + (keysym - XK_a);
				}
				else if (keysym >= XK_0 && keysym <= XK_9) {
					vk = '0' + (keysym - XK_0);
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
				if (event.xclient.data.l[0] == wmDeleteMessage){
					self->m_EventQueue.push(CWindow::QuitEvent{});
				}
				break;
		}
	}
	#elif defined(WEB_PLT)
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

auto CWindow::show() -> void
{
	#if defined(WINDOWS_PLT)
	ShowWindow(m_Handle, SW_SHOW);
	#elif defined(LINUX_PLT)
	XMapWindow(m_Surface, m_Handle);
	#endif
	m_Visible = true;
}
auto CWindow::hide() -> void
{
	#if defined(WINDOWS_PLT)
	ShowWindow(m_Handle, SW_HIDE);
	#elif defined(LINUX_PLT)
	XUnmapWindow(m_Surface, m_Handle);
	#endif
	m_Visible = false;
}

auto CWindow::toggle_fullscreen() -> void
{
	#if defined(WINDOWS_PLT)
	struct WindowedState
	{
		RECT rect;
		DWORD style, exStyle;
	};

	static const TCHAR* FULLSCREEN_STATE_PROP = TEXT("FullscreenStateData");

	if (m_FullScreen)
	{
		WindowedState state;
		UINT_PTR stored = reinterpret_cast<UINT_PTR>(RemoveProp(m_Handle, FULLSCREEN_STATE_PROP));
		if (stored) {
			state.style = static_cast<DWORD>(stored & 0xFFFFFFFF);
			state.exStyle = static_cast<DWORD>(stored >> 32);
	
			RECT currentRect;
			GetWindowRect(m_Handle, &currentRect);
	
			SetWindowLongPtr(m_Handle, GWL_STYLE, state.style);
			SetWindowLongPtr(m_Handle, GWL_EXSTYLE, state.exStyle);
			
			SetWindowPos(
				m_Handle, nullptr,
				currentRect.left, currentRect.top,
				currentRect.right - currentRect.left,
				currentRect.bottom - currentRect.top,
				SWP_FRAMECHANGED | SWP_NOZORDER
			);
		}
		m_FullScreen = false;
	}
	else
	{
		WindowedState state;
		state.style = static_cast<DWORD>(GetWindowLongPtr(m_Handle, GWL_STYLE));
		state.exStyle = static_cast<DWORD>(GetWindowLongPtr(m_Handle, GWL_EXSTYLE));
		
		UINT_PTR storedState = (static_cast<UINT_PTR>(state.exStyle) << 32) | state.style;
	
		HMONITOR hmon = MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi {};
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hmon, &mi);
	
		SetWindowLongPtr(m_Handle, GWL_STYLE, state.style & ~(WS_CAPTION | WS_THICKFRAME));
		SetWindowLongPtr(m_Handle, GWL_EXSTYLE, state.exStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));
	
		SetWindowPos(
			m_Handle, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_NOZORDER
		);
	
		SetProp(m_Handle, FULLSCREEN_STATE_PROP, reinterpret_cast<HANDLE>(storedState));
		m_FullScreen = true;
	}

	#elif defined(LINUX_PLT)
	//not impl yet
	#elif defined(WEB_PLT)

	EmscriptenFullscreenChangeEvent fullscrendata;
    EMSCRIPTEN_RESULT ret = emscripten_get_fullscreen_status(&fullscrendata);

	if (!fullscrendata.isFullscreen) {
		ret = emscripten_request_fullscreen(m_Surface, 1);
	} else {
		ret = emscripten_exit_fullscreen();
	}

	#endif
}

auto CWindow::swap_buffers() const -> void
{
	#if defined(WINDOWS_PLT)
    ::SwapBuffers(m_Surface);
    #elif defined(LINUX_PLT)
    ::glXSwapBuffers(m_Surface, m_Handle);
    #endif
}

auto CWindow::close() -> void
{
    hide();
    debug::print("Exit. ");
}

auto CWindow::get_title() -> std::string 
{
	std::string title;

	#if defined(WINDOWS_PLT)
	int len = GetWindowTextLength(m_Handle);
    title.resize(len + 1); // room for null

    // GetWindowText returns number of characters copied (excluding null)
    if (GetWindowText(m_Handle, &title[0], static_cast<int>(title.size())) > 0)
        title.resize(len); // remove the extra null char

	#elif defined(LINUX_PLT)
	// brocken
	char* name = nullptr;
    if (XFetchName(m_Surface, m_Handle, &name) && name) {
		title.resize(std::strlen(name) + 1);
        XFree(name);
	}
	#elif defined(WEB_PLT)
	title = emscripten_get_window_title();
	#endif

	return title;
}

auto CWindow::set_title(std::string  title) -> void
{
	#if defined(WINDOWS_PLT)
	SetWindowText(m_Handle, title.c_str()); 
	#elif defined(LINUX_PLT)
    XStoreName(m_Surface, m_Handle, title.c_str());
	#elif defined(WEB_PLT)
	emscripten_set_window_title(title.c_str());
	#endif
}

auto CWindow::set_vsync(bool state) -> void
{
	
	#if defined(WINDOWS_PLT)
	if(!wglSwapIntervalEXT) wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
	wglSwapIntervalEXT(state);

	#elif defined(LINUX_PLT)
	if(!glXSwapIntervalEXT) glXSwapIntervalEXT = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT"));
	glXSwapIntervalEXT(m_Surface, m_Handle, state);

	#elif defined(WEB_PLT)
	(void)state;
	debug::print("vSync is always enabled in web and no vSync off ");
	#endif
}
