#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/gl.h>
extern "C" {
    using wglCreateContextAttribsARB_type = HGLRC(WINAPI*)(HDC hdc, HGLRC hShareContext, const int *attribList);
    wglCreateContextAttribsARB_type wglCreateContextAttribsARB = nullptr;

    using wglChoosePixelFormatARB_type = BOOL(WINAPI*)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    wglChoosePixelFormatARB_type wglChoosePixelFormatARB = nullptr;

    void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar *message, [[maybe_unused]] const void *param)
    {
        const char *source_, *type_, *severity_;

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:             source_ = "API";             break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_ = "WINDOW_SYSTEM";   break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_ = "SHADER_COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_ = "THIRD_PARTY";     break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_ = "APPLICATION";     break;
        case GL_DEBUG_SOURCE_OTHER:           source_ = "OTHER";           break;
        default:                              source_ = "<SOURCE>";        break;
        }

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:               type_ = "ERROR";               break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_ = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_ = "UDEFINED_BEHAVIOR";   break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_ = "PORTABILITY";         break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_ = "PERFORMANCE";         break;
        case GL_DEBUG_TYPE_OTHER:               type_ = "OTHER";               break;
        case GL_DEBUG_TYPE_MARKER:              type_ = "MARKER";              break;
        default:                                type_ = "<TYPE>";              break;
        }

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         severity_ = "HIGH";         break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severity_ = "MEDIUM";       break;
        case GL_DEBUG_SEVERITY_LOW:          severity_ = "LOW";          break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; break;
        default:                             severity_ = "<SEVERITY>";   break;
        }
        Log::print("[{} {}({})] From {} : \n\t- {}", severity_, type_, id, source_, message);
    }
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

auto OpenGL::init_opengl_dummy() -> void
{
    // Register a dummy window class.
    const char* dummyClassName = "DummyOpenGLWindowClass";
    WNDCLASSA wc = {};
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = DefWindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = dummyClassName;

    if (!RegisterClassA(&wc)) {
        Log::Error("Failed to register dummy window class. Error code: {}", GetLastError());
        return;
    }
    HWND dummy_window = CreateWindowExA(
        0,
        dummyClassName,
        "Dummy",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr,
        wc.hInstance,
        nullptr);

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
    #define RESOLVEGL(type, name) name = reinterpret_cast<type>(rsgl(#name))
	GLFUNCS(RESOLVEGL)

    glGetIntegerv(GL_MAJOR_VERSION, &OpenGL::vMajor);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGL::vMinor);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);

    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if( OpenGL::MajorV() >= 4 && OpenGL::MinorV() >= 3 && (flags & GL_CONTEXT_FLAG_DEBUG_BIT)){
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLDebugMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    GLint max_texture_unit = 0, nGlslv = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_unit);
    glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &nGlslv);

    Log::print("GL Version({}.{}) : {}", OpenGL::MajorV(), OpenGL::MinorV(), reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Log::print("GLSL Version : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    Log::print("Number of GLSL Version Supported : {}", nGlslv);
    for(GLint i = 0; i < nGlslv; i++) 
        Log::print("    [{}]- {}", i+1, reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i)));

    Log::print("GL Vendor : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    Log::print("GL Renderer : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    Log::print("Max Texture Units : {}", max_texture_unit);
    Log::print("Is Debugable : {}", flags && GL_CONTEXT_FLAG_DEBUG_BIT);
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
    init_opengl_dummy();

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
   

    int gl_attribs[] = { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 4,
        WGL_CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
        WGL_CONTEXT_PROFILE_MASK_ARB, 0x00000001, // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
        0,
    };

    if (GetLastError() == ERROR_INVALID_VERSION_ARB){
        Log::Error("Unsupported GL Version {}.{}", gl_attribs[1], gl_attribs[3]);
    }
    
    HGLRC gl33_context = wglCreateContextAttribsARB(m_MainHDC, 0, gl_attribs);
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