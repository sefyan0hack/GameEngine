#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/gl.h>

static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar *message, [[maybe_unused]] const void *param)
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


auto __GetProcAddress(LPCSTR module, const char* name) -> void* {
    auto lib = LoadLibraryA(module);
    if(lib == nullptr){
        Log::Error("Couldnt load lib opengl32 reason: {}", GetLastError());
        return nullptr;
    }

    void *address = (void *)GetProcAddress(lib, name);
    if(address != nullptr)
    {
        return address;
    }else{
        Log::Error("Couldnt load function `{}` from module : {}", name, module);
        return nullptr;
    }
}
auto rsgl(const char* name) -> void* {
    void *address = (void *)_wglGetProcAddress(name);

    if(address == nullptr
    || address == reinterpret_cast<void*>(0x1)
    || address == reinterpret_cast<void*>(0x2)
    || address == reinterpret_cast<void*>(0x3)
    || address == reinterpret_cast<void*>(-1))
    {
        address = __GetProcAddress(OpenGL::OPENGL_MODULE_NAME, name);
        if(address == nullptr){
            Log::Error("Couldnt load opengl function `{}` reason: {}", name, GetLastError());
        }  
    }

    Log::Info("load opengl function `{}` at : {}", name, address);
    return address;
}

OpenGL::OpenGL(HWND window)
    : m_MainHDC(GetDC(window))
{
    if( m_MainHDC == nullptr){
        Log::Error("HDC not valid");
    }

    init_opengl(window);
    glGetIntegerv(GL_MAJOR_VERSION, &vMajor);
    glGetIntegerv(GL_MINOR_VERSION, &vMinor);

    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
 
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if( OpenGL::MajorV() >= 4 && OpenGL::MinorV() >= 3 && (flags & GL_CONTEXT_FLAG_DEBUG_BIT)){
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
    Log::print("GL Exts : {}", wglGetExtensionsStringARB(m_MainHDC));
    Log::print("Max Texture Units : {}", max_texture_unit);
    Log::print("Is Debugable : {}", !!(flags & GL_CONTEXT_FLAG_DEBUG_BIT));
}

OpenGL::OpenGL(const OpenGL &other)
    : m_MainHDC(other.m_MainHDC)
    , vMajor(other.vMajor)
    , vMinor(other.vMinor)
{
    auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) Log::Error("couldn't Copy Opengl Context");
}

auto OpenGL::operator=(const OpenGL &other) -> OpenGL
{
    this->m_MainHDC = other.m_MainHDC;
    this->vMajor = other.vMajor;
    this->vMinor = other.vMinor;
    auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) Log::Error("couldn't Copy Opengl Context");

    return *this;
}
OpenGL::~OpenGL()
{
    _wglMakeCurrent(nullptr, nullptr);
    _wglDeleteContext(m_Context);
}

auto OpenGL::init_opengl(HWND window) -> void
{

    PIXELFORMATDESCRIPTOR pfd {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    auto pixel_format = ChoosePixelFormat(m_MainHDC, &pfd);
    if (!pixel_format) {
        Log::Error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(m_MainHDC, pixel_format, &pfd)) {
        Log::Error("Failed to set the pixel format.");
    }

    _wglMakeCurrent     = (decltype(_wglMakeCurrent))__GetProcAddress(OPENGL_MODULE_NAME, "wglMakeCurrent");
    _wglCreateContext   = (decltype(_wglCreateContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCreateContext");
    _wglGetProcAddress  = (decltype(_wglGetProcAddress))__GetProcAddress(OPENGL_MODULE_NAME, "wglGetProcAddress");
    _wglDeleteContext   = (decltype(_wglDeleteContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglDeleteContext");
    _wglCopyContext     = (decltype(_wglCopyContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCopyContext");

    if (!_wglMakeCurrent
        || !_wglCreateContext
        || !_wglGetProcAddress
        || !_wglDeleteContext) {
        Log::Error("Failed to load required WGL function.");
    }


    HGLRC dummy_context = nullptr;

    dummy_context = _wglCreateContext(m_MainHDC);
    if (!dummy_context) {
        Log::Error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!_wglMakeCurrent(m_MainHDC, dummy_context)) {
        Log::Error("Failed to activate dummy OpenGL rendering context.");
    }

    wglCreateContextAttribsARB  = (decltype(wglCreateContextAttribsARB))rsgl("wglCreateContextAttribsARB");
    wglGetExtensionsStringARB   = (decltype(wglGetExtensionsStringARB))rsgl("wglGetExtensionsStringARB");

    if (!wglGetExtensionsStringARB || !wglCreateContextAttribsARB) {
        Log::Error("Failed to load required WGL extensions.");
    }

    int gl_attribs[] = { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 4,
        WGL_CONTEXT_FLAGS_ARB, 0x00000004, // WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
        WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, 0x8252, // WGL_LOSE_CONTEXT_ON_RESET_ARB
        WGL_CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
        WGL_CONTEXT_PROFILE_MASK_ARB, 0x00000001, // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
        0,
    };


    HGLRC opengl_context = nullptr;
    if (nullptr == (opengl_context = wglCreateContextAttribsARB(m_MainHDC, nullptr, gl_attribs))) {
        _wglDeleteContext(dummy_context);
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            Log::Error("Unsupported GL Version {}.{}", gl_attribs[1], gl_attribs[3]);
        }
        Log::Error("Failed to create the final rendering context!");
    }

    _wglMakeCurrent(m_MainHDC, opengl_context); // conseder to make it curent in window.hpp maybe
    _wglDeleteContext(dummy_context);

    m_Context =  opengl_context;

    //resolve opengl functions
    #define RESOLVEGL(type, name)\
    name = reinterpret_cast<type>(rsgl(#name));\
    Log::Expect(name != nullptr, "{} is null", #name)

	GLFUNCS(RESOLVEGL)

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

auto OpenGL::isValid() -> bool
{
    return not m_Context;
}
