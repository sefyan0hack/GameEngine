#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/gl.h>
#include <core/Utils.hpp>

[[maybe_unused]] static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar *message, [[maybe_unused]] const void *param)
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
    , m_Context(nullptr)
    , m_Major(0)
    , m_Minor(0)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(false)
{
    if( m_MainHDC == nullptr){
        Log::Error("HDC not valid");
    }

    init_opengl();
    glGetIntegerv(GL_MAJOR_VERSION, &m_Major);
    glGetIntegerv(GL_MINOR_VERSION, &m_Minor);
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    m_Debug = !!(flags & GL_CONTEXT_FLAG_DEBUG_BIT);
    
    static bool isInitialized = false;
    if (!isInitialized) {
        m_Vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        m_Renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        m_Extensions = split(reinterpret_cast<const char*>(wglGetExtensionsStringARB(m_MainHDC)), " ");

        GLint nGlslv = 0;
        glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &nGlslv);

        for(GLint i = 0; i < nGlslv; i++){
            auto r = reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i));
            if(r) m_GlslVersions.push_back(r);
        }

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_MaxTextureUnits);
    }
    

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


    // #ifdef DEBUG
    // if( m_Major >= 4 && m_Minor >= 3 && m_Debug){
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(GLDebugMessageCallback, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // }
    // #endif

    Log::print("GL Version : {}.{}", m_Major, m_Minor);
    Log::print("GLSL Version Supported : {}", to_string(m_GlslVersions));
    Log::print("GL Vendor : {}", m_Vendor);
    Log::print("GL Renderer : {}", m_Renderer);
    Log::print("GL Exts : {}", to_string(m_Extensions));
    Log::print("Max Texture Units : {}", m_MaxTextureUnits);
    Log::print("Debug : {}", m_Debug ? "On" : "Off");
}

OpenGL::OpenGL(const OpenGL &other)
    : m_MainHDC(other.m_MainHDC)
    , m_Context(nullptr)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(other.m_Debug)
{
    auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) Log::Error("couldn't Copy Opengl Context");
}

auto OpenGL::operator=(const OpenGL &other) -> OpenGL
{
    if(*this != other){
        this->m_MainHDC = other.m_MainHDC;
        this->m_Context = nullptr;
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = std::time(nullptr);
        this->m_Debug = other.m_Debug;

        auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        if(tst != TRUE) Log::Error("couldn't Copy Opengl Context");
    }
    return *this;
}

OpenGL::OpenGL(OpenGL &&other) noexcept
    : m_MainHDC(other.m_MainHDC)
    , m_Context(other.m_Context)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(other.m_CreationTime)
    , m_Debug(other.m_Debug)
{
    other.m_MainHDC = nullptr;
    other.m_Context = nullptr;
    other.m_Major = 0;
    other.m_Minor = 0;
    other.m_CreationTime = 0;
    other.m_Debug = false;
}

auto OpenGL::operator=(OpenGL &&other) noexcept -> OpenGL
{
    if(*this != other){
        this->m_MainHDC = other.m_MainHDC;
        this->m_Context = other.m_Context;
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = other.m_CreationTime;
        this->m_Debug = other.m_Debug;
        
        other.m_MainHDC = nullptr;
        other.m_Context = nullptr;
        other.m_Major = 0;
        other.m_Minor = 0;
        other.m_CreationTime = 0;
        other.m_Debug = false;

    }

    return *this;
}

auto OpenGL::operator == (const OpenGL& other) const -> bool
{
    return this->m_Context == other.m_Context;
}

auto OpenGL::operator != (const OpenGL& other) const ->bool
{
    return !(*this == other);
}

OpenGL::operator bool() const
{
    return isValid();
}

OpenGL::~OpenGL()
{
    _wglMakeCurrent(nullptr, nullptr);
    _wglDeleteContext(m_Context);
}

auto OpenGL::init_opengl() -> void
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

    glGetError = reinterpret_cast<decltype(glGetError)>(rsgl("glGetError"));

    #ifdef DEBUG
    #   define RESOLVEGL(type, name)\
        name = glFunction<type>{};\
        name.m_Func = reinterpret_cast<type>(rsgl(#name));\
        name.m_Name = #name;
    #else
    #   define RESOLVEGL(type, name)\
        name = reinterpret_cast<type>(rsgl(#name))
    #endif

	GLFUNCS(RESOLVEGL)
}

auto OpenGL::DrawContext() const -> HDC
{
    return m_MainHDC;
}

auto OpenGL::MajorV() const -> GLint
{
    return m_Major;
}
auto OpenGL::MinorV() const -> GLint
{
    return m_Minor;
}

auto OpenGL::isValid() const -> bool
{
    return not m_Context;
}

auto OpenGL::CreationTime() const -> std::time_t
{
    return m_CreationTime;
}

auto OpenGL::isDebugable() const -> bool
{
    return m_Debug;
}

auto OpenGL::Vendor() -> std::string
{
    return m_Vendor;
}
auto OpenGL::Renderer() -> std::string
{
    return m_Renderer;
}

auto OpenGL::GlslVersions() -> std::vector<std::string>
{
    return m_GlslVersions;
}

auto OpenGL::Extensions() -> std::vector<std::string>
{
    return m_Extensions;
}

auto OpenGL::MaxTextureUnits() -> GLint
{
    return m_MaxTextureUnits;
}
