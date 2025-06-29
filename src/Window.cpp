#include <core/Window.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>


CWindow::CWindow([[maybe_unused]] int32_t Width, [[maybe_unused]] int32_t Height, [[maybe_unused]] const char* Title, [[maybe_unused]] bool withopengl) 
	: m_Width(Width)
	, m_Height(Height)
	, m_Visible(false)
	, m_refCount(1)
	, m_Keyboard(std::make_shared<Keyboard>())
	, m_Mouse(std::make_shared<Mouse>())
{
	#if defined(WINDOWS_PLT)
	_init_helper(m_Width, m_Height, Title);
	m_DrawContext = GetDC(m_WindowHandle);
	#elif defined(LINUX_PLT)
	m_DrawContext = XOpenDisplay(nullptr);
	_init_helper(m_Width, m_Height, Title);
	#elif defined(WEB_PLT)
	m_DrawContext = EMSCRIPTEN_EVENT_TARGET_DOCUMENT;
	_init_helper(m_Width, m_Height, Title);
	#endif
	
	S_WindowsCount++;

	if(withopengl) m_OpenGl = std::make_shared<gl::OpenGL>(m_WindowHandle, m_DrawContext);

	#if defined(WEB_PLT)
	auto target = EMSCRIPTEN_EVENT_TARGET_WINDOW;

	emscripten_set_keypress_callback(target, this, EM_FALSE, &CWindow::KeyHandler);
	emscripten_set_keydown_callback(target, this, EM_FALSE, &CWindow::KeyHandler);
	emscripten_set_keyup_callback(target, this, EM_FALSE, &CWindow::KeyHandler);
	
	//
	emscripten_set_mousedown_callback(target , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mouseup_callback(target    , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mousemove_callback(target  , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mouseenter_callback(target , this, EM_FALSE, &CWindow::MouseHandler);
	emscripten_set_mouseleave_callback(target , this, EM_FALSE, &CWindow::MouseHandler);

	emscripten_set_focusout_callback(target, this, EM_FALSE,
		[](int32_t eventType, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
			CWindow* window = static_cast<CWindow*>(userData);
    		if (!window) return EM_TRUE;
			switch (eventType)
			{
				case EMSCRIPTEN_EVENT_BLUR:
					window->m_Keyboard->ClearState();
					break;
			}
			return EM_TRUE;
		}
	);

	emscripten_set_wheel_callback(target, this, EM_FALSE,
		[]([[maybe_unused]] int32_t eventType, const EmscriptenWheelEvent* e, void* userData) -> EM_BOOL  {
			CWindow* w = static_cast<CWindow*>(userData);
			w->m_Mouse->OnWheelDelta(e->deltaY * -120); // Match Win32 scaling
			return EM_TRUE;
		}
	);

	emscripten_set_fullscreenchange_callback("#canvas", this, EM_FALSE, 
		[](
			int32_t eventType, 
			const EmscriptenFullscreenChangeEvent* e,
			void* userData
		) -> EM_BOOL {
			auto* window = static_cast<CWindow*>(userData);

			if (e->isFullscreen) Info("Enable FullScreen");

			window->m_Width = e->elementWidth;
			window->m_Height = e->elementHeight;

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
	, m_Keyboard(other.m_Keyboard)
	, m_Mouse(other.m_Mouse)
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
    m_WinclassEx.lpfnWndProc = CWindow::WinProcSetup;
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

auto CWindow::WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    if (msg == WM_NCCREATE){
        const CREATESTRUCTW* WinptrStruct = reinterpret_cast<CREATESTRUCTW*>(Lpr);
        CWindow* const pWin  = reinterpret_cast<CWindow*>(WinptrStruct->lpCreateParams);
        SetWindowLongPtrA( Winhandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWin) );
		SetWindowLongPtrA( Winhandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CWindow::WinProcSetup2) );

		return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CWindow::WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
	CWindow* const pWnd = reinterpret_cast<CWindow*>(GetWindowLongPtrA( Winhandle, GWLP_USERDATA ));

	return pWnd->WinProcFun( Winhandle, msg, Wpr, Lpr );
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
            int32_t ret = MessageBoxA(m_WindowHandle, "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
            if (ret == IDYES){
                --S_WindowsCount;
                m_Visible = false;
                ShowWindow(Winhandle, HIDE_WINDOW);
                PostQuitMessage(0); //hmmmm
                Info("Exit. ");
            }
            return 0;
        }
        case WM_SIZE:{
            m_Width  = LOWORD(Lpr);
            m_Height = HIWORD(Lpr);
			gl::Viewport(0, 0, m_Width, m_Height);
            return 0;
        }
        /*********** KEYBOARD MESSAGES ***********/
	    case WM_KEYDOWN:
	    // syskey commands need to be handled to track ALT key (VK_MENU) and F10
	    case WM_SYSKEYDOWN:
		if( !(Lpr & 0x40000000) || m_Keyboard->AutorepeatIsEnabled() ) // filter autorepeat
	    	{
	    		m_Keyboard->OnKeyPressed( static_cast<unsigned char>(Wpr) );
	    	}
	    	break;
	    case WM_KEYUP:
	    case WM_SYSKEYUP:
		m_Keyboard->OnKeyReleased( static_cast<unsigned char>(Wpr) );
	    	break;
	    case WM_CHAR:
	    	m_Keyboard->OnChar( static_cast<char>(Wpr) );
	    	break;
	    ///////////// END KEYBOARD MESSAGES /////////////

        ///////////// MOUSE MESSAGES /////////////////
	    case WM_MOUSEMOVE:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
			if( !m_Mouse->IsInWindow() )
			{
				m_Mouse->OnMouseEnter();
			}
			if( pt.x >= 0 &&  pt.x < m_Width && pt.y >= 0 && pt.y < m_Height )
			{
				m_Mouse->OnMouseMove( pt.x, pt.y );
				m_Mouse->OnMouseEnter();
			}
			else{
				m_Mouse->OnMouseLeave();
			}
			return 0;
	    }
		case WM_MOUSEHOVER :{
			m_Mouse->isEntered = true;
			return 0;
		}
		case WM_MOUSELEAVE :{
			m_Mouse->isEntered = false;
			return 0;
		}
	    case WM_LBUTTONDOWN:
	    {
	    	SetForegroundWindow( Winhandle );
	    	m_Mouse->OnLeftPressed();
	    	break;
	    }
	    case WM_RBUTTONDOWN:
	    {
	    	m_Mouse->OnRightPressed();
	    	break;
	    }
	    case WM_LBUTTONUP:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
	    	m_Mouse->OnLeftReleased();
	    	// release mouse if outside of window
	    	if( pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height )
	    	{
	    		ReleaseCapture();
	    		m_Mouse->OnMouseLeave();
	    	}
	    	break;
	    }
	    case WM_RBUTTONUP:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
	    	m_Mouse->OnRightReleased();
	    	// release mouse if outside of window
	    	if( pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height )
	    	{
	    		ReleaseCapture();
	    		m_Mouse->OnMouseLeave();
	    	}
	    	break;
	    }
	    case WM_MOUSEWHEEL:
	    {
	    	const int32_t delta = GET_WHEEL_DELTA_WPARAM( Wpr );
	    	m_Mouse->OnWheelDelta(delta);
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
	    		m_Mouse->OnRawDelta( ri.data.mouse.lLastX,ri.data.mouse.lLastY );
	    	}
	    	break;
	    }
	    ///////////////// END RAW MOUSE MESSAGES /////////////////
        case WM_KILLFOCUS:
		m_Keyboard->ClearState();
		ClipCursor(nullptr); //release cursor confinement
		break;

		case WM_SETCURSOR:
		if (m_Mouse->isLocked && LOWORD(Lpr) == HTCLIENT) {
			SetCursor(nullptr);
			return true;
		}
		break;
		

    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CWindow::_init_helper(int32_t Width, int32_t Height, const char* Title) -> void
{
    WinClass::Instance();

    RECT WinRect = { 0, 0, Width, Height };

	if( AdjustWindowRect( &WinRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE ) == 0 )
	{
		Error("Addjusting Win");
	}
    m_Width = WinRect.right - WinRect.left;
    m_Height = WinRect.bottom - WinRect.top;

    m_WindowHandle = CreateWindow(
        MAKEINTATOM(WinClass::m_Windclass),
        Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, m_Width, m_Height,
        nullptr, nullptr,
        GetModuleHandleA( nullptr ),
        this
    );

    if(m_WindowHandle == nullptr){
        Error("faild to creat CWindow code : {}", GetLastError());
        return;
    }
	// regester mouse raw data
	RAWINPUTDEVICE _rid;
	_rid.dwFlags = 0;
	_rid.usUsagePage  = 0x01;
	_rid.usUsage  = 0x02;
	_rid.hwndTarget = nullptr;
	if(RegisterRawInputDevices(&_rid, 1, sizeof(_rid)) == false){
		Error("Mouse row data not regesterd");
	}

}

#elif defined(LINUX_PLT)

auto CWindow::_init_helper(int32_t Width, int32_t Height, const char* Title) -> void
{
    if (!m_DrawContext) {
		Error("Failed to open X display connection.");
	}
    int32_t screen = DefaultScreen(m_DrawContext);

    /* Create a window */
    m_WindowHandle = XCreateSimpleWindow(m_DrawContext, RootWindow(m_DrawContext, screen), 
                                 10, 10, static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1, 
                                 BlackPixel(m_DrawContext, screen), WhitePixel(m_DrawContext, screen));
    Expect(m_WindowHandle != 0, "m_WindowHandle are null ???");
    XStoreName(m_DrawContext, m_WindowHandle, Title);

	XkbSetDetectableAutoRepeat(m_DrawContext, true, NULL);
    /* Select input events */
    XSelectInput(m_DrawContext, m_WindowHandle,
		 KeyPressMask | KeyReleaseMask | ExposureMask |
		 ResizeRedirectMask | FocusChangeMask | StructureNotifyMask
		);

    /* Show the window */
    // XMapWindow(m_DrawContext, m_WindowHandle);
}

#elif defined(WEB_PLT)
auto CWindow::_init_helper(int32_t Width, int32_t Height, const char* Title) -> void
{
	emscripten_set_window_title(Title);
	emscripten_set_canvas_element_size("#canvas", Width, Height);
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

	// auto safeCopy = [](const char* src) -> std::string {
    //     return src ? std::string(src) : "";
    // };
	
	// std::string key = safeCopy(e->key);
    // std::string code = safeCopy(e->code);
    // std::string locale = safeCopy(e->locale);

	// Info(
	// 	R"(timestamp: {}, key: "{}", code: {}, location: {}, Flags: [ctrlKey:{}, shiftKey:{}, altKey:{}, metaKey:{}, repeat:{}], locale: "{}")",
	// 	e->timestamp, key, code, e->location, e->ctrlKey, e->shiftKey, e->altKey, e->metaKey, e->repeat, locale
	// );

    auto MapToVirtualKey = [](const char* code) -> unsigned char {
        // Alphanumeric keys
        if (strlen(code) == 4 && code[0] == 'K' && code[1] == 'e' && code[2] == 'y') 
            return code[3];  // 'A'-'Z'
        if (strlen(code) == 6 && !strncmp(code, "Digit", 5)) 
            return code[5];  // '0'-'9'

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
        
        return 0;  // Unmapped key
    };

    auto MapToChar = [](const char* key) -> char {
        if (strlen(key) == 1) return key[0];  // Printable characters
        if (strcmp(key, "Enter") == 0) return '\r';
        if (strcmp(key, "Tab") == 0) return '\t';
        if (strcmp(key, "Backspace") == 0) return '\b';
        if (strcmp(key, "Escape") == 0) return '\x1B';
        return 0;  // Non-character key
    };

    switch (eventType) {
        case EMSCRIPTEN_EVENT_KEYDOWN: {
            unsigned char vk = MapToVirtualKey(e->code);
            if (vk != 0) {
                window->m_Keyboard->OnKeyPressed(vk);
            }

            char ch = MapToChar(e->key);
            if (ch != 0) {
                window->m_Keyboard->OnChar(ch);
            }
        } break;

        case EMSCRIPTEN_EVENT_KEYUP: {
            unsigned char vk = MapToVirtualKey(e->code);
            if (vk != 0) {
                window->m_Keyboard->OnKeyReleased(vk);
            }
        } break;
    }

    return EM_TRUE;
}

auto CWindow::MouseHandler( int32_t eventType, const EmscriptenMouseEvent* e, void* userData) -> EM_BOOL
{
    CWindow* window = static_cast<CWindow*>(userData);
    if (!window) return EM_TRUE;

	// Info(
	// 	R"(screen({},{}), client({},{}), movement({},{}), target({},{}), canvas({},{}), button: {})",
	// 	e->screenX, e->screenY, e->clientX, e->clientY, e->movementX, e->movementY, e->targetX, e->targetY, e->canvasX, e->canvasY,
	// 	e->button == 0 ? "Left" :
	// 	e->button == 1 ? "Mid"  :
	// 	e->button == 2 ? "Right": "??"
	// );

	switch (eventType) {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            if (e->button == 0) window->m_Mouse->OnLeftPressed();
            else if (e->button == 2) window->m_Mouse->OnRightPressed();
            break;
        
        case EMSCRIPTEN_EVENT_MOUSEUP:
            if (e->button == 0) window->m_Mouse->OnLeftReleased();
            else if (e->button == 2) window->m_Mouse->OnRightReleased();
            break;
        
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
			window->m_Mouse->OnRawDelta(e->movementX, e->movementY);
			window->m_Mouse->OnMouseMove(e->canvasX, e->canvasY);
            break;
        
        case EMSCRIPTEN_EVENT_MOUSEENTER:
            window->m_Mouse->OnMouseEnter();
            break;
        
        case EMSCRIPTEN_EVENT_MOUSELEAVE:
            window->m_Mouse->OnMouseLeave();
            break;
    }

    return EM_FALSE;
}
#endif

auto CWindow::ProcessMessages([[maybe_unused]] CWindow* self) -> void
{
	Expect(self != nullptr, "Cwindow* self Can't be null");

	// auto This = reinterpret_cast<CWindow*>(self);
	
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
				self->m_Width = event.xconfigure.width;
				self->m_Height = event.xconfigure.height;
				break;

			case KeyPress:
			case KeyRelease: {
				const KeyCode keycode = event.xkey.keycode;
				if (event.type == KeyPress) {
					keyboard.OnKeyPressed(keycode);

					char buffer[32];
					KeySym keysym;
					XComposeStatus compose;
					int char_count = XLookupString(&event.xkey, buffer, sizeof(buffer), &keysym, &compose);

					for (int i = 0; i < char_count; ++i) {
						keyboard.OnChar(static_cast<unsigned char>(buffer[i]));
					}

				} else {
					keyboard.OnKeyReleased(keycode);
				}
				break;
			}

			case FocusOut:
				// Clear keyboard state when window loses focus
				self->m_Keyboard->ClearState();
				break;

			case ClientMessage:
				if (event.xclient.data.l[0] == wmDeleteMessage){
					Info("Close Window");
					S_WindowsCount--;
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