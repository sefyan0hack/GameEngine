#include "Window.hpp"
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>
#include <bit>

#include <windows.h>

#if !defined(CONSOLE_ATTACHED)
extern int main(int, char**);
int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
    return main(__argc, __argv);
}
#endif

extern auto from_native(WPARAM key, LPARAM Lpr) -> Key;

CWindow::~CWindow()
{
	ReleaseDC(m_Handle, m_Surface);
	DestroyWindow(m_Handle);
}

static auto CALLBACK win_proc_thunk(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT
{
    switch (msg)
    {
        case WM_CREATE:{
            return 0;
        }
        case WM_CLOSE:{
			EventQ::self().push(CWindow::QuitEvent{});
			return 0;
        }
        case WM_SIZE:{
			EventQ::self().push(CWindow::ResizeEvent{LOWORD(Lpr), HIWORD(Lpr)});
            return 0;
        }
        /*********** KEYBOARD MESSAGES ***********/
	    case WM_KEYDOWN:
	    case WM_SYSKEYDOWN:
			EventQ::self().push(Keyboard::KeyDownEvent{from_native(Wpr, Lpr)});
			break;
		
		case WM_KEYUP:
		case WM_SYSKEYUP:
			EventQ::self().push(Keyboard::KeyUpEvent{from_native(Wpr, Lpr)});
			break;

	    ///////////// END KEYBOARD MESSAGES /////////////

        ///////////// MOUSE MESSAGES /////////////////
	    case WM_MOUSEMOVE:
	    {
			EventQ::self().push(Mouse::MoveEvent{static_cast<SHORT>(LOWORD(Lpr)), static_cast<SHORT>(HIWORD(Lpr))});
			return 0;
	    }
		case WM_MOUSEHOVER :{
			EventQ::self().push(Mouse::EnterEvent{});
			return 0;
		}
		case WM_MOUSELEAVE :{
			EventQ::self().push(Mouse::LeaveEvent{});
			return 0;
		}
	    case WM_LBUTTONDOWN:
			EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Left});
			break;
		case WM_MBUTTONDOWN:
			EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Middle});
			break;
	    case WM_RBUTTONDOWN:
			EventQ::self().push(Mouse::ButtonDownEvent{Mouse::Button::Right});
			break;
		case WM_LBUTTONUP:
			EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::Left});
			break;
		case WM_MBUTTONUP:
			EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::Middle});
			break;
	    case WM_RBUTTONUP:
	    	EventQ::self().push(Mouse::ButtonUpEvent{Mouse::Button::Right});
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
				EventQ::self().push(Mouse::MovementEvent{
					static_cast<float>(ri.data.mouse.lLastX),
					static_cast<float>(ri.data.mouse.lLastY)
				});
	    	}
	    	break;
	    }
	    ///////////////// END RAW MOUSE MESSAGES /////////////////
		// TODO: Handle focus events  now this function is static
		// case WM_SETFOCUS:
		// 	EventQ::self().push(CWindow::SetFocusEvent{this});
		// 	break;
        // case WM_KILLFOCUS:
		// 	EventQ::self().push(CWindow::LoseFocusEvent{this});
		// 	ClipCursor(nullptr); //release cursor confinement
		// 	break;

    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}


const TCHAR* window_class_name()
{
	static constexpr const TCHAR* WINDOW_CLASS_NAME = TEXT("MyEngineMainWindowClass");

    WNDCLASSEX wc = {};

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = win_proc_thunk;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (RegisterClassEx(&wc) == 0) {
        DWORD error = GetLastError();

		if(error != ERROR_CLASS_ALREADY_EXISTS)
			throw Exception("Failed to register window class: {}", error);
    }
    
    return WINDOW_CLASS_NAME;
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
{

    auto window_handle = CreateWindow(
        window_class_name(),
        Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
        nullptr, nullptr,
        GetModuleHandleA( nullptr ),
        this
    );

    if(window_handle == nullptr){
        throw Exception("faild to creat CWindow code : {}", GetLastError());
    }

	return {0 /*display*/, window_handle, GetDC(window_handle)};
}

auto CWindow::process_messages() -> void
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
	
		SetProp(m_Handle, FULLSCREEN_STATE_PROP, std::bit_cast<HANDLE>(storedState));
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
	BEGIN_IGNORE_WARN_CALNG("-Wcast-function-type-mismatch")
	if(!gl::SwapIntervalEXT) gl::SwapIntervalEXT = reinterpret_cast<decltype(gl::SwapIntervalEXT)>(gl::GetProcAddress("wglSwapIntervalEXT"));
	END_IGNORE_WARN_CALNG()
	gl::SwapIntervalEXT(state);
}

auto CWindow::message_box(const char* title, const char* body) -> bool
{
	return MessageBox(m_Handle, body, title, MB_YESNO) == IDYES;
}

auto CWindow::dims() const	-> std::pair<int32_t, int32_t>
{
	RECT rect{};
	if(GetWindowRect(m_Handle, &rect))
		return { rect.right - rect.left, rect.bottom - rect.top };
	else
		throw Exception("GetWindowRect failed with: {}", GetLastError());
}

auto CWindow::resize(int32_t width, int32_t height)	-> void
{
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	SetWindowPos(
		m_Handle,
		nullptr,
		0, 0,
		rc.right - rc.left, 
		rc.bottom - rc.top, 
    	SWP_NOMOVE | SWP_NOZORDER
	);
}
