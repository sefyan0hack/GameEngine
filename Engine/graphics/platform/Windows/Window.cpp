#include "Window.hpp"
#include "gl.hpp"

#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>

#include <windows.h>

extern int main(int, char**);
int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
    return main(__argc, __argv);
}

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
    return DefWindowProc(Winhandle, msg, Wpr, Lpr);
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
        GetModuleHandle( nullptr ),
        this
    );

    if(window_handle == nullptr){
        throw Exception("faild to creat CWindow code : {}", GetLastError());
    }

	return {0 /*display*/, window_handle, GetDC(window_handle)};
}

auto CWindow::poll_events() -> void
{
    MSG Msg = {};
    while (PeekMessage(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
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

	intptr_t style{}, exStyle{};
	int64_t x{}, y{}, width{}, height{};

	constexpr auto style_prp_name = TEXT("Winstyle");
	constexpr auto exStyle_prp_name = TEXT("WinexStyle");
	constexpr auto x_prp_name = TEXT("Winx");
	constexpr auto y_prp_name = TEXT("Winy");
	constexpr auto width_prp_name = TEXT("Winwidth");
	constexpr auto height_prp_name = TEXT("Winheight");

	if (m_FullScreen)
	{
		// TODO: check if RemoveProp != null
		style = reinterpret_cast<intptr_t>(RemoveProp(m_Handle, style_prp_name));
		exStyle = reinterpret_cast<intptr_t>(RemoveProp(m_Handle, exStyle_prp_name));
		x = reinterpret_cast<int64_t>(RemoveProp(m_Handle, x_prp_name));
		y = reinterpret_cast<int64_t>(RemoveProp(m_Handle, y_prp_name));
		width = reinterpret_cast<int64_t>(RemoveProp(m_Handle, width_prp_name));
		height = reinterpret_cast<int64_t>(RemoveProp(m_Handle, height_prp_name));

		SetWindowLongPtr(m_Handle, GWL_STYLE, style);
		SetWindowLongPtr(m_Handle, GWL_EXSTYLE, exStyle);

		SetWindowPos(
			m_Handle, nullptr,
			static_cast<int32_t>(x), static_cast<int32_t>(y),
			static_cast<int32_t>(width),
			static_cast<int32_t>(height),
			SWP_FRAMECHANGED | SWP_NOZORDER
		);

		m_FullScreen = false;

	} else {

		style = GetWindowLongPtr(m_Handle, GWL_STYLE);
		exStyle = GetWindowLongPtr(m_Handle, GWL_EXSTYLE);

		RECT rect{};
		GetWindowRect(m_Handle, &rect);

		HMONITOR hmon = MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi {};
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hmon, &mi);
	
		SetWindowLongPtr(m_Handle, GWL_STYLE, style & ~(WS_CAPTION | WS_THICKFRAME));
		SetWindowLongPtr(m_Handle, GWL_EXSTYLE, exStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));
	
		SetWindowPos(
			m_Handle, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_NOZORDER
		);
	
		auto ret = SetProp(m_Handle, style_prp_name, reinterpret_cast<HANDLE>(style));
		if(ret == 0) throw Exception("SetProp failed: {}", GetLastError());

		ret = SetProp(m_Handle, exStyle_prp_name, reinterpret_cast<HANDLE>(exStyle));
		if(ret == 0) throw Exception("SetProp failed: {}", GetLastError());

		ret = SetProp(m_Handle, x_prp_name, reinterpret_cast<HANDLE>(rect.left));
		if(ret == 0) throw Exception("SetProp failed: {}", GetLastError());

		ret = SetProp(m_Handle, y_prp_name, reinterpret_cast<HANDLE>(rect.top));
		if(ret == 0) throw Exception("SetProp failed: {}", GetLastError());

		ret = SetProp(m_Handle, width_prp_name, reinterpret_cast<HANDLE>(rect.right - rect.left));
		if(ret == 0) throw Exception("SetProp failed: {}", GetLastError());

		ret = SetProp(m_Handle, height_prp_name, reinterpret_cast<HANDLE>(rect.bottom - rect.top));
		if(ret == 0) throw Exception("SetProp failed: {}", GetLastError());

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
	//TODO: needs gl context
	// WGL_EXT_swap_control
	GET_GLEXT_FUNCTION_THROW(wglSwapIntervalEXT);
	wglSwapIntervalEXT_ext(state);
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
