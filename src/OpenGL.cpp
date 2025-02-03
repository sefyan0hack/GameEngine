#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/gl.h>
extern "C"{
    using wglCreateContextAttribsARB_type = HGLRC(WINAPI*)(HDC hdc, HGLRC hShareContext, const int *attribList);
    wglCreateContextAttribsARB_type wglCreateContextAttribsARB = nullptr;

    using wglChoosePixelFormatARB_type = BOOL(WINAPI*)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    wglChoosePixelFormatARB_type wglChoosePixelFormatARB = nullptr;
}

auto rsgl(const char* name) -> void* {
    void *address = (void *)wglGetProcAddress(name);

    if(address == nullptr
    || address == reinterpret_cast<void*>(0x1)
    || address == reinterpret_cast<void*>(0x2)
    || address == reinterpret_cast<void*>(0x3)
    || address == reinterpret_cast<void*>(-1))
    {
        static auto lib = LoadLibraryA("opengl32.dll");
        if(lib == nullptr){
            Log::Error("Couldnt load lib opengl32 reason: {}", GetLastError());
        }
        
        address = (void *)GetProcAddress(lib, name);
        
        if(address == nullptr){
            Log::Error("Couldnt load  opengl function `{}` reason: {}", name, GetLastError());
        }else{
            Log::Info("opengl function by LoadLibraryA `{}` at : {}", name, address);
            return address;
        }
        
    }else{
        Log::Info("opengl function by wglGetProcAddress `{}` at : {}", name, address);
        return address;
    }
}

auto OpenGL::init_opengl_extensions() -> void
{
    HWND dummy_window = CreateWindowExA(
        0,
        "STATIC",
        "Dummy",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0,
        NULL,
        0);

    if (dummy_window != nullptr) {

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
        Log::Error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        Log::Error("Failed to set the pixel format.");
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        Log::Error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        Log::Error("Failed to activate dummy OpenGL rendering context.");
    }

    auto ProcwglCreateContextAttribsARB = wglGetProcAddress("wglCreateContextAttribsARB");
    auto ProcwglChoosePixelFormatARB = wglGetProcAddress("wglChoosePixelFormatARB");
    
    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type)ProcwglCreateContextAttribsARB;
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type)ProcwglChoosePixelFormatARB;

    if (!wglCreateContextAttribsARB || !wglChoosePixelFormatARB) {
        Log::Error("Failed to load required WGL extensions.");
    }

    if( wglMakeCurrent(dummy_dc, 0) == false
    ||  wglDeleteContext(dummy_context) == false
    ||  ReleaseDC(dummy_window, dummy_dc) == false
    ||  DestroyWindow(dummy_window) == false){
        Log::Error("Destruction of init_opengl_extention failed");
    }
    }else {
        Log::Error("Failed to create dummy OpenGL window. {}", GetLastError());
    }
}

OpenGL::OpenGL(HWND window)
{
    static bool once = false;
    if(!once){
        m_MainHDC = GetDC(window);
        init_opengl();
        once = true;
    }

    //resolve opengl functions
    #define RESOLVEGL(type, name) name = (type)rsgl(#name)
	GLFUNCS(RESOLVEGL)

    glGetIntegerv(GL_MAJOR_VERSION, &OpenGL::vMajor);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGL::vMajor);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);

    GLint max_texture_unit = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_unit);
    Log::print("GL Version : {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Log::print("GLSL Version : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    Log::print("GL Vendor : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    Log::print("GL Renderer : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    Log::print("Max Texture Units : {}", max_texture_unit);
}

OpenGL::~OpenGL()
{
    wglMakeCurrent(nullptr, nullptr);
    ReleaseDC (m_MainWindow, m_MainHDC);
    wglDeleteContext(m_Context);
}

auto OpenGL::init_opengl() -> void
{

    if( m_MainHDC == nullptr){
        Log::Error("HDC not valid");
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

    int pixel_format{};
    UINT num_formats{};
    wglChoosePixelFormatARB(m_MainHDC, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);

    if (!num_formats || !m_MainHDC) {
        Log::Error("Failed to set the OpenGL 3.3 pixel format. {}", GetLastError());
    }else{
        if (GetPixelFormat(m_MainHDC) != pixel_format) {
            PIXELFORMATDESCRIPTOR pfd;
            if (!DescribePixelFormat(m_MainHDC, pixel_format, sizeof(pfd), &pfd)) {
                Log::Error("DescribePixelFormat failed.");
            }

            if (!SetPixelFormat(m_MainHDC, pixel_format, &pfd)) {
                Log::Error("Failed to set the OpenGL 3.3 pixel format.");
            }
        }
    }
   

    int gl33_attribs[] = { 
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };
    
    HGLRC gl33_context = wglCreateContextAttribsARB(m_MainHDC, 0, gl33_attribs);
    if (!gl33_context) {
        Log::Error("Failed to create OpenGL 3.3 context.");
    }

    if (!wglMakeCurrent(m_MainHDC, gl33_context)) {
        Log::Error("Failed to activate OpenGL 3.3 rendering context.");
    }

    m_Context =  gl33_context;
}

auto OpenGL::GetHDC() const -> HDC
{
    return m_MainHDC;
}

auto OpenGL::MajorV() -> GLint
{
    return vMajor;
}
auto OpenGL::MinorV() -> GLint
{
    return vMinor;
}