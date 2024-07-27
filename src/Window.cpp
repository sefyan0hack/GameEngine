#include "Window.hpp"
#include <cassert>
// #include <iostream>
#include "Global_H.hpp"
#include "G_vars.hpp"

extern "C"{
    typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int *attribList);
    wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

    typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;
}
POINT Window::GetMousePosition() const{
    return m_MousePos;
}
LRESULT CALLBACK Window::WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr){
    switch (msg)
    {
        case WM_KEYDOWN:{
            switch (Wpr)
            {
            case VK_LEFT:
                xyz[0] -= 100.0f * 1/(GLfloat)fps.QuadPart;
                return 0;
            case VK_UP:
                xyz[1] += 100.0f * 1/(GLfloat)fps.QuadPart;
                return 0;
            case VK_DOWN:
                xyz[1] -= 100.0f * 1/(GLfloat)fps.QuadPart;
                return 0;
            case VK_RIGHT:
                xyz[0] += 100.0f * 1/(GLfloat)fps.QuadPart;
                return 0;
            }
            return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
        }
        case WM_CREATE:{
            LOG("Creat Main Window");
            m_HDC = GetDC(Winhandle);
            init_opengl();
            return 0;
        }
        case WM_CLOSE:
        case WM_DESTROY:{
            int ret = MessageBoxA(Winhandle, "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
            if (ret == IDYES){
                PostQuitMessage(0);
                LOG("Exit.");
            }
            return 0;
        }
        case WM_SIZE:{
            m_Width  = LOWORD(Lpr);
            m_Height = HIWORD(Lpr);
            
            return 0;
        }
        case WM_MOUSEMOVE:{
            m_MousePos = {LOWORD(Lpr), HIWORD(Lpr)};
            return 0;
        }
        case WM_SETCURSOR:{
            switch (LOWORD(Lpr))
            {
                case HTCLOSE:
                case HTMINBUTTON:
                case HTMAXBUTTON:
                default:
                    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
                }
            return 0;
        }
        case WM_PAINT:{
            glViewport(0, 0, m_Width, m_Height);
            glClearColor(0.f, 0.3f, 0.6f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // glUniform1f(randFromShaderLoc, Rand_float);
            
            // // glUseProgram(shaderProgram);
            // // glBindVertexArray(VAO);
            // glDrawArrays(GL_TRIANGLES, 0, 3);
            // SwapBuffers(m_HDC);
            return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
        }
    }
    return DefWindowProcA(Winhandle, msg, Wpr, Lpr);
}

Window::Window()
{
    _init_helper();
}

Window::~Window()
{
    ReleaseDC(m_WindowHandle, m_HDC);
    UnregisterClassA(CLASS_NAME, m_Instance);
}
bool Window::ShouldWindowClose() const{
    if(!Runing){
        return true;
    }else{
        return false;
    }
}
void Window::ProcessMessages()
{
    MSG Msg = {};
    while (PeekMessageA(&Msg, m_WindowHandle, 0u, 0u, PM_REMOVE))
    {
        if(Msg.message == WM_QUIT){
            Runing = false;
            return;
        }
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
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


void Window::init_opengl_extensions()
{
    static WNDCLASSA window_class {};
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = DefWindowProcA;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = "Dummy_WGL_djuasiodwa";

    if (!RegisterClassA(&window_class) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        ERR("Failed to register dummy OpenGL window.");
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0,
        window_class.hInstance,
        0);

    if (!dummy_window) {
        ERR("Failed to create dummy OpenGL window.");
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) {
        ERR("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        ERR("Failed to set the pixel format.");
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        ERR("Failed to create a dummy OpenGL rendering context.");
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        ERR("Failed to activate dummy OpenGL rendering context.");
    }

    auto ProcwglCreateContextAttribsARB = wglGetProcAddress("wglCreateContextAttribsARB");
    auto ProcwglChoosePixelFormatARB = wglGetProcAddress("wglChoosePixelFormatARB");
    
    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)ProcwglCreateContextAttribsARB;
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)ProcwglChoosePixelFormatARB;

    if (!wglCreateContextAttribsARB || !wglChoosePixelFormatARB) {
        ERR("Failed to load required WGL extensions.");
    }

    if( wglMakeCurrent(dummy_dc, 0) == false
    ||  wglDeleteContext(dummy_context) == false
    ||  ReleaseDC(dummy_window, dummy_dc) == false
    ||  DestroyWindow(dummy_window) == false){
        ERR("Destruction of init_opengl_extention failed");
    }
}

void Window::init_opengl()
{

    if( m_HDC == nullptr){
        ERR("HDC not valid");
    }
    init_opengl_extensions();

    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(m_HDC, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);

    if (!num_formats) {
        ERR("Failed to set the OpenGL 3.3 pixel format.");
    }

    if (GetPixelFormat(m_HDC) != pixel_format) {
        PIXELFORMATDESCRIPTOR pfd;
        if (!DescribePixelFormat(m_HDC, pixel_format, sizeof(pfd), &pfd)) {
            ERR("DescribePixelFormat failed.");
        }

        if (!SetPixelFormat(m_HDC, pixel_format, &pfd)) {
            ERR("Failed to set the OpenGL 3.3 pixel format.");
        }
    }

    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC gl33_context = wglCreateContextAttribsARB(m_HDC, 0, gl33_attribs);
    if (!gl33_context) {
        ERR("Failed to create OpenGL 3.3 context.");
    }

    if (!wglMakeCurrent(m_HDC, gl33_context)) {
        ERR("Failed to activate OpenGL 3.3 rendering context.");
    }
    gladLoadGL();
    LOG("GL Version : " << glGetString(GL_VERSION));
    LOG("GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG("GL Vendor : " << glGetString(GL_VENDOR));
    LOG("GL Renderer : " << glGetString(GL_RENDERER));

    m_OpenGL_Context =  gl33_context;
}

void Window::_init_helper(){
    windclass.lpfnWndProc = Window::WinProcFun;

    if(RegisterClassExA(&windclass) == 0){
        ERR("faild to regester class");
        return;
    }
    m_WindowHandle = CreateWindowExA(
        0,
        windclass.lpszClassName,
        "Main",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr,
        m_Instance,
        nullptr
    );

    if(m_WindowHandle == nullptr){
        ERR("faild to creat Window code : " << GetLastError());
        return;
    }
    m_HDC = GetDC(m_WindowHandle);
    UpdateWindow(m_WindowHandle);
}

Window & Window::InstanceWindow() {
    static Window InsWindow;
    return InsWindow;
}
