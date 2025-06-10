#include <core/Window.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#endif

CWindow::CWindow([[maybe_unused]] int Width, [[maybe_unused]] int Height, [[maybe_unused]] const char* Title) 
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
	_init_helper(m_Width, m_Height, Title);
	#endif

    S_WindowsCount++;
    m_OpenGl = std::make_shared<gl::OpenGL>(m_WindowHandle, m_DrawContext);

	#if defined(WEB_PLT)
    // emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, ResizeHandler);
    // emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, KeyHandler);
    // emscripten_set_mousedown_callback("#canvas", this, 1, MouseHandler);
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
            int ret = MessageBoxA(m_WindowHandle, "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
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
			m_Mouse->isEnterd = true;
			return 0;
		}
		case WM_MOUSELEAVE :{
			m_Mouse->isEnterd = false;
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
	    	const int delta = GET_WHEEL_DELTA_WPARAM( Wpr );
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
		break;
		

    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto CWindow::ProcessMessages() -> void
{
	#if defined(WINDOWS_PLT)
    MSG Msg = {};
    while (PeekMessageA(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
	#elif defined(LINUX_PLT)
	int screen = DefaultScreen(m_DrawContext);

    /* Event loop */
    // while (true) {
    //     XNextEvent(m_DrawContext, &m_Event);
    //     if (m_Event.type == Expose) {
    //         XFillRectangle(m_DrawContext, m_WindowHandle, DefaultGC(m_DrawContext, screen), 20, 20, 200, 150);
    //     }
    //     if (m_Event.type == KeyPress) {
    //         S_WindowsCount--;
    //         break; //press Exit on key 
    //     }
    // }
	#endif
}
auto CWindow::_init_helper(int Width, int Height, const char* Title) -> void
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

auto CWindow::_init_helper(int Width, int Height, const char* Title) -> void
{
    if (!m_DrawContext) {
		Error("Failed to open X display connection.");
	}
    int screen = DefaultScreen(m_DrawContext);

    /* Create a window */
    m_WindowHandle = XCreateSimpleWindow(m_DrawContext, RootWindow(m_DrawContext, screen), 
                                 10, 10, static_cast<unsigned int>(Width), static_cast<unsigned int>(Height), 1, 
                                 BlackPixel(m_DrawContext, screen), WhitePixel(m_DrawContext, screen));
    Expect(m_WindowHandle != 0, "m_WindowHandle are null ???");
    XStoreName(m_DrawContext, m_WindowHandle, Title);

    /* Select input events */
    XSelectInput(m_DrawContext, m_WindowHandle, ExposureMask | KeyPressMask);

    /* Show the window */
        XMapWindow(m_DrawContext, m_WindowHandle);
}

#elif defined(WEB_PLT)
auto CWindow::_init_helper(int Width, int Height, const char* Title) -> void
{
	EM_ASM({
		const canvas = document.getElementById("canvas");
        document.title = UTF8ToString($0);
        canvas.width = $1;
        canvas.height = $2;
    }, Title, Width, Height);
}

int CWindow::ResizeHandler([[maybe_unused]] int eventType, [[maybe_unused]] const EmscriptenUiEvent* e, [[maybe_unused]] void* userData) {
    [[maybe_unused]] CWindow* window = static_cast<CWindow*>(userData);
    window->m_Width  = e->windowInnerWidth;
    window->m_Height = e->windowInnerHeight;

	EM_ASM(
	{
		const canvas = document.getElementById("canvas");
		canvas.Height = $0;
		canvas.width = $1;
	}, window->m_Width, window->m_Height);

    return 1;
}

int CWindow::KeyHandler([[maybe_unused]] int eventType, [[maybe_unused]] const EmscriptenKeyboardEvent* e, [[maybe_unused]] void* userData) {
    [[maybe_unused]] CWindow* window = static_cast<CWindow*>(userData);
    // Handle key events (e.key, e.code, etc.)
    return 1;
}

int CWindow::MouseHandler([[maybe_unused]] int eventType, [[maybe_unused]] const EmscriptenMouseEvent* e, [[maybe_unused]] void* userData) {
    [[maybe_unused]] CWindow* window = static_cast<CWindow*>(userData);
    // Handle mouse events (e.clientX, e.clientY, e.button, etc.)
    return 1;
}
#endif

auto CWindow::WindowHandle() const -> WindHandl
{
    return m_WindowHandle;
}

auto CWindow::DrawContext() const -> HDC_D
{
    return m_DrawContext;
}

auto CWindow::Width() const -> int
{
    return m_Width;
}

auto CWindow::Height() const -> int
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
