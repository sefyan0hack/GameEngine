#include <core/Window.hpp>
#include <core/Log.hpp>
#include <core/gl.h>

// Window class things///////////////////////////////////
Window::WinClass &Window::WinClass::Instance()
{
    static Window::WinClass ClassIns; 
    return ClassIns;
}
auto Window::WinClass::Name() -> const char*
{
    return m_Name;
}
Window::WinClass::WinClass(){
    m_Winclass = {
        .cbSize = sizeof(WNDCLASSEXA),
        .style = CS_GLOBALCLASS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS,
        .lpfnWndProc = Window::WinProcSetup,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance =  GetModuleHandleA(nullptr),
        .hIcon = LoadIconA(nullptr, IDI_APPLICATION),
        .hCursor = LoadCursorA(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = "",
        .lpszClassName = m_Name,
        .hIconSm = LoadIcon(nullptr, IDI_APPLICATION),
    };
    if(RegisterClassExA(&m_Winclass) == 0){
        Log::Error("faild to regester class {}", GetLastError());
    }
}
Window::WinClass::~WinClass()
{
    UnregisterClassA(m_Name, GetModuleHandleA(nullptr));
}
///////////////////////////////////////////////////////////////////

auto Window::WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    if (msg == WM_NCCREATE){
        const CREATESTRUCTW* WinptrStruct = reinterpret_cast<CREATESTRUCTW*>(Lpr);
        Window* const pWin  = reinterpret_cast<Window*>(WinptrStruct->lpCreateParams);
        SetWindowLongPtrA( Winhandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWin) );
		SetWindowLongPtrA( Winhandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WinProcSetup2) );

		return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto Window::WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtrA( Winhandle, GWLP_USERDATA ));

	return pWnd->WinProcFun( Winhandle, msg, Wpr, Lpr );
}
auto CALLBACK Window::WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    switch (msg)
    {
        case WM_CREATE:{
            Log::Info("Creat Main Window");
            return 0;
        }
        case WM_CLOSE:{
            int ret = MessageBoxA(nullptr, "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
            if (ret == IDYES){
                --S_WindowsCount;
                m_Visible = false;
                ShowWindow(Winhandle, HIDE_WINDOW);
                PostQuitMessage(0); //hmmmm
                Log::Info("Exit. ");
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
	    	if( !(Lpr & 0x40000000) || kbd.AutorepeatIsEnabled() ) // filter autorepeat
	    	{
	    		kbd.OnKeyPressed( static_cast<unsigned char>(Wpr) );
	    	}
	    	break;
	    case WM_KEYUP:
	    case WM_SYSKEYUP:
	    	kbd.OnKeyReleased( static_cast<unsigned char>(Wpr) );
	    	break;
	    case WM_CHAR:
	    	kbd.OnChar( static_cast<char>(Wpr) );
	    	break;
	    ///////////// END KEYBOARD MESSAGES /////////////

        ///////////// MOUSE MESSAGES /////////////////
	    case WM_MOUSEMOVE:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
			if( !mouse.IsInWindow() )
			{
				mouse.OnMouseEnter();
			}
			if( pt.x >= 0 && pt.x < m_Width && pt.y >= 0 && pt.y < m_Height )
			{
				mouse.OnMouseMove( pt.x, pt.y );
				mouse.OnMouseEnter();
			}
			else{
				mouse.OnMouseLeave();
			}
			return 0;
	    }
		case WM_MOUSEHOVER :{
			mouse.isEnterd = true;
			return 0;
		}
		case WM_MOUSELEAVE :{
			mouse.isEnterd = false;
			return 0;
		}
	    case WM_LBUTTONDOWN:
	    {
	    	SetForegroundWindow( Winhandle );
	    	mouse.OnLeftPressed();
	    	break;
	    }
	    case WM_RBUTTONDOWN:
	    {
	    	mouse.OnRightPressed();
	    	break;
	    }
	    case WM_LBUTTONUP:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
	    	mouse.OnLeftReleased();
	    	// release mouse if outside of window
	    	if( pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height )
	    	{
	    		ReleaseCapture();
	    		mouse.OnMouseLeave();
	    	}
	    	break;
	    }
	    case WM_RBUTTONUP:
	    {
	    	const POINTS pt = MAKEPOINTS( Lpr );
	    	mouse.OnRightReleased();
	    	// release mouse if outside of window
	    	if( pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height )
	    	{
	    		ReleaseCapture();
	    		mouse.OnMouseLeave();
	    	}
	    	break;
	    }
	    case WM_MOUSEWHEEL:
	    {
	    	const int delta = GET_WHEEL_DELTA_WPARAM( Wpr );
	    	mouse.OnWheelDelta(delta);
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
	    	rawBuffer.resize( size );
	    	// read in the input data
	    	if( GetRawInputData(
	    		reinterpret_cast<HRAWINPUT>(Lpr),
	    		RID_INPUT,
	    		rawBuffer.data(),
	    		&size,
	    		sizeof( RAWINPUTHEADER ) ) != size )
	    	{
	    		//my log error hire
	    		break;
	    	}
	    	// process the raw input data
	    	auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
	    	if( ri.header.dwType == RIM_TYPEMOUSE &&
	    		(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0) )
	    	{
	    		mouse.OnRawDelta( ri.data.mouse.lLastX,ri.data.mouse.lLastY );
	    	}
	    	break;
	    }
	    ///////////////// END RAW MOUSE MESSAGES /////////////////
        case WM_KILLFOCUS:
		    kbd.ClearState();
		    break;


    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

auto Window::WindowsCount() -> unsigned short
{
    return S_WindowsCount;
}
Window::Window(int m_Width, int m_Height, const char* Title) 
: m_Instance( GetModuleHandleA( nullptr ) ), m_Visible(true)
{
    _init_helper(m_Width, m_Height, Title);
    S_WindowsCount++;
    m_OpenGl = std::make_unique<OpenGL>(m_WindowHandle);
}

Window::~Window()
{
    DestroyWindow(m_WindowHandle);
}
auto Window::ProcessMessages() -> void
{
    MSG Msg = {};
    while (PeekMessageA(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
}

auto Window::_init_helper(int Width, int Height, const char* Title) -> void
{
    WinClass::Instance();

    RECT WinRect;
	WinRect.left = 100;
	WinRect.right = Width + WinRect.left;
	WinRect.top = 100;
	WinRect.bottom = Height + WinRect.top;
	if( AdjustWindowRect( &WinRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE ) == 0 )
	{
		Log::Error("Addjusting Win");
	}
    m_Width = WinRect.right - WinRect.left;
    m_Height = WinRect.bottom - WinRect.top;

    m_WindowHandle = CreateWindowExA(
        0,
        WinClass::Name(),
        Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, m_Width, m_Height,
        nullptr, nullptr,
        m_Instance,
        this
    );

    if(m_WindowHandle == nullptr){
        Log::Error("faild to creat Window code : {}", GetLastError());
        return;
    }
	// regester mouse raw data
	RAWINPUTDEVICE _rid;
	_rid.dwFlags = 0;
	_rid.usUsagePage  = 0x01;
	_rid.usUsage  = 0x02;
	_rid.hwndTarget = nullptr;
	if(RegisterRawInputDevices(&_rid, 1, sizeof(_rid)) == false){
		Log::Error("Mouse row data not regesterd");
	}
    
    ShowWindow(m_WindowHandle, SW_SHOW);
    UpdateWindow(m_WindowHandle);
}

auto Window::GetHINSTANCE() const -> HINSTANCE
{
    return m_Instance;
}

auto Window::GetHWND() const -> HWND
{
    return m_WindowHandle;
}

auto Window::GetHDC() const -> HDC
{
    return m_HDC;
}

auto Window::GetWidth() const -> int
{
    return m_Width;
}

auto Window::GetHeight() const -> int
{
    return m_Height;
}
auto Window::GetGL() const -> OpenGL*
{
    return m_OpenGl.get();
}
auto Window::Visible() const -> bool
{
    return m_Visible;
}