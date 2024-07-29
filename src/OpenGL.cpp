#include "OpenGL.hpp"
#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END
extern "C"{
    using wglCreateContextAttribsARB_type = HGLRC(WINAPI*)(HDC hdc, HGLRC hShareContext, const int *attribList);
    wglCreateContextAttribsARB_type wglCreateContextAttribsARB = nullptr;

    using wglChoosePixelFormatARB_type = BOOL(WINAPI*)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    wglChoosePixelFormatARB_type wglChoosePixelFormatARB = nullptr;
}

void OpenGL::init_opengl_extensions()
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
    
    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type)ProcwglCreateContextAttribsARB;
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type)ProcwglChoosePixelFormatARB;

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

OpenGL::OpenGL(HWND window): m_window(window), m_HDC(GetDC(window)) // check in fut if hwnd valid
{
    init_opengl();
}

OpenGL::~OpenGL()
{
    wglMakeCurrent(nullptr, nullptr);
    ReleaseDC (m_window, m_HDC) ; 
    wglDeleteContext(m_Context);
}

void OpenGL::init_opengl()
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
    
    static bool on = false;
    if(on == false){
        gladLoadGL();
        on = true;
    }
    LOG("GL Version : " << glGetString(GL_VERSION));
    LOG("GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG("GL Vendor : " << glGetString(GL_VENDOR));
    LOG("GL Renderer : " << glGetString(GL_RENDERER));
    m_Context =  gl33_context;
}

HDC OpenGL::GetHDC() const
{
    return m_HDC;
}
