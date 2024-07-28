#include "Window.hpp"
#include "Global_H.hpp"

// Window class things///////////////////////////////////
Window::WinClass &Window::WinClass::Instance()
{
    static Window::WinClass ClassIns; 
    return ClassIns;
}
const char* Window::WinClass::Title(){
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
        .hIconSm = LoadIconA(nullptr, IDI_APPLICATION),
    };
    if(RegisterClassExA(&m_Winclass) == 0){
        ERR("faild to regester class " << GetLastError());
    }
}
Window::WinClass::~WinClass()
{
    UnregisterClassA(m_Name, GetModuleHandleA(nullptr));
}
///////////////////////////////////////////////////////////////////

LRESULT Window::WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)
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

LRESULT Window::WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtrA( Winhandle, GWLP_USERDATA ));

	return pWnd->WinProcFun( Winhandle, msg, Wpr, Lpr );
}
LRESULT CALLBACK Window::WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr){
    switch (msg)
    {
        case WM_CREATE:{
            LOG("Creat Main Window");
            return 0;
        }
        case WM_CLOSE:{
            int ret = MessageBoxA(nullptr, "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
            if (ret == IDYES){
                --S_WindowsCount;
                m_Visible = false;
                ShowWindow(Winhandle, HIDE_WINDOW);
                PostQuitMessage(0); //hmmmm
                LOG("Exit. ");
            }
            return 0;
        }
        case WM_SIZE:{
            m_Width  = LOWORD(Lpr);
            m_Height = HIWORD(Lpr);
            return 0;
        }

    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

unsigned short Window::WindowsCount()
{
    return S_WindowsCount;
}
Window::Window(int Width, int Height, const char* Title) : m_Instance( GetModuleHandleA( nullptr ) ), m_Visible(true)
{
    _init_helper(Width, Height, Title);
    S_WindowsCount++;
    m_OpenGl = std::make_unique<OpenGL>(m_WindowHandle);
}

Window::~Window()
{
    ReleaseDC(m_WindowHandle, m_HDC);
    DestroyWindow(m_WindowHandle);
}
void Window::ProcessMessages()
{
    MSG Msg = {};
    while (PeekMessageA(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
}


void Window::_init_helper(int Width, int Height, const char* Title){
    WinClass::Instance();
    RECT WinRect;
	WinRect.left = 100;
	WinRect.right = Width + WinRect.left;
	WinRect.top = 100;
	WinRect.bottom = Height + WinRect.top;
	if( AdjustWindowRect( &WinRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE ) == 0 )
	{
		ERR("Addjusting Win");
	}
    m_Width = WinRect.right - WinRect.left;
    m_Height = WinRect.bottom - WinRect.top;

    m_WindowHandle = CreateWindowExA(
        0,
        WinClass::Title(),
        Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, m_Width, Height,
        nullptr, nullptr,
        m_Instance,
        this
    );

    if(m_WindowHandle == nullptr){
        ERR("faild to creat Window code : " << GetLastError());
        return;
    }
    
    m_HDC = GetDC(m_WindowHandle);
    ShowWindow(m_WindowHandle, SW_SHOW);
    UpdateWindow(m_WindowHandle);
}

HINSTANCE Window::GetHINSTANCE() const{
    return m_Instance;
}

HWND Window::GetHWND() const{
    return m_WindowHandle;
}

HDC Window::GetHDC() const{
    return m_HDC;
}

int Window::GetWidth() const
{
    return m_Width;
}

int Window::GetHeight() const
{
    return m_Height;
}
bool Window::Visible() const
{
    return m_Visible;
}