#include <core/XWindow.hpp>
#include <core/Log.hpp>
#include <core/gl.h>

#ifdef __linux__
#include <X11/Xlib.h>

auto CWindow::WindowsCount() -> unsigned short
{
    return S_WindowsCount;
}
CWindow::CWindow(int m_Width, int m_Height, const char** Title) 
	: m_Display(nullptr)
	, m_Visible(true)
	, m_Keyboard(std::make_shared<Keyboard>())
	, m_Mouse(std::make_shared<Mouse>())
{
    _init_helper(m_Width, m_Height, Title);
    S_WindowsCount++;
    m_OpenGl = std::make_shared<OpenGL>(m_WindowHandle);
}

CWindow::CWindow(const Window& other)
	: m_Display(other.m_Display)
	, m_WindowHandle(other.m_WindowHandle)
	, m_Width(other.m_Width)
	, m_Height(other.m_Height)
	, m_Visible(other.m_Visible)
	, m_RawBuffer(other.m_RawBuffer)
	, m_OpenGl(other.m_OpenGl)
	, m_Keyboard(other.m_Keyboard)
	, m_Mouse(other.m_Mouse)
{
}

CWindow::~Window()
{
    XDestroyWindow(m_Display, m_WindowHandle);
    XCloseDisplay(m_Display);
}

auto CWindow::ProcessMessages() -> void
{
    /* Event loop */
    while (true) {
        XNextEvent(m_Display, &m_Event);
        if (m_Event.type == Expose) {
            XFillRectangle(m_Display, m_WindowHandle, DefaultGC(m_Display, screen), 20, 20, 200, 150);
        }
        if (m_Event.type == KeyPress) {
            S_WindowsCount--;
            break; //press Exit on key 
        }
    }
}

auto CWindow::_init_helper(int Width, int Height, const char* Title) -> void
{
    int screen;

    /* Open connection to X server */
    m_Display = XOpenDisplay(nullptr);
    if (m_Display == nullptr) {
        Error("Cannot open display");
    }

    screen = DefaultScreen(m_Display);

    /* Create a window */
    m_WindowHandle = XCreateSimpleWindow(m_Display, RootWindow(m_Display, screen), 
                                 10, 10, 800, 600, 1, 
                                 BlackPixel(m_Display, screen), WhitePixel(m_Display, screen));
    
    XStoreName(m_Display, m_WindowHandle, Title);

    /* Select input events */
    XSelectInput(m_Display, m_WindowHandle, ExposureMask | KeyPressMask);

    /* Show the window */
    if(m_Visible)
        XMapWindow(m_Display, m_WindowHandle);
}

CWindow::~CWindow(){
    XCloseDisplay(m_Display);
}

auto Window::WindowHandle() const -> Window
{
    return m_WindowHandle;
}

auto Window::Width() const -> int
{
    return m_Width;
}

auto Window::Height() const -> int
{
    return m_Height;
}
auto Window::opengl() const -> std::shared_ptr<OpenGL>
{
    return m_OpenGl;
}
auto Window::Visible() const -> bool
{
    return m_Visible;
}

#endif //__linux__
