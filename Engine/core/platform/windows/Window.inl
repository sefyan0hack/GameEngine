#include <cstring>

#include "Window.hpp"
#include "Log.hpp"
#include "OpenGL.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "EventQueue.hpp"


CWindow::~CWindow()
{
	DestroyWindow(m_Handle);
}

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

auto CWindow::process_messages([[maybe_unused]] CWindow* self) -> void
{
    MSG Msg = {};
    while (PeekMessageA(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }	
}

auto CWindow::show() -> void
{
	ShowWindow(m_Handle, SW_SHOW);
	m_Visible = true;
}

auto CWindow::hide() -> void
{
	ShowWindow(m_Handle, SW_HIDE);
}

auto CWindow::toggle_fullscreen() -> void
{
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
}

auto CWindow::swap_buffers() const -> void
{
    ::SwapBuffers(m_Surface);
}


auto CWindow::get_title() -> std::string 
{
	std::string title;

    int len = GetWindowTextLength(m_Handle);
    title.resize(len + 1); // room for null

    // GetWindowText returns number of characters copied (excluding null)
    if (GetWindowText(m_Handle, &title[0], static_cast<int>(title.size())) > 0){
        title.resize(len); // remove the extra null char
	}

	return title;
}

auto CWindow::set_title(std::string  title) -> void
{
	SetWindowText(m_Handle, title.c_str()); 
}

auto CWindow::set_vsync(bool state) -> void
{
	if(!wglSwapIntervalEXT) wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
	wglSwapIntervalEXT(state);
}
