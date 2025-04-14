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

    Info("[{} {}({})] From {} : \n\t- {}", severity_, type_, id, source_, message);
}

#ifdef _WIN32
auto __GetProcAddress(LPCSTR module, const char* name) -> void* {
    auto lib = LoadLibraryA(module);
    if(lib == nullptr){
        Error("Couldnt load lib opengl32 reason: {}", GetLastError());
        return nullptr;
    }

    void *address = (void *)GetProcAddress(lib, name);
    if(address != nullptr)
    {
        return address;
    }else{
        Error("Couldnt load function `{}` from module : {}", name, module);
        return nullptr;
    }
}
auto rsgl(const char* name) -> void* {
    void *address = (void *)_wglGetProcAddress(name);

    if(address == nullptr
    or address == reinterpret_cast<void*>(0x1)
    or address == reinterpret_cast<void*>(0x2)
    or address == reinterpret_cast<void*>(0x3)
    or address == reinterpret_cast<void*>(-1))
    {
        address = __GetProcAddress(OpenGL::OPENGL_MODULE_NAME, name);
        if(address == nullptr){
            Error("Couldnt load opengl function `{}` reason: {}", name, GetLastError());
        }  
    }

    Info("load opengl function `{}` at : {}", name, address);
    return address;
}


auto OpenGL::init_opengl_win32() -> void
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
        Error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(m_MainHDC, pixel_format, &pfd)) {
        Error("Failed to set the pixel format.");
    }

    _wglMakeCurrent     = (decltype(_wglMakeCurrent))__GetProcAddress(OPENGL_MODULE_NAME, "wglMakeCurrent");
    _wglCreateContext   = (decltype(_wglCreateContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCreateContext");
    _wglGetProcAddress  = (decltype(_wglGetProcAddress))__GetProcAddress(OPENGL_MODULE_NAME, "wglGetProcAddress");
    _wglDeleteContext   = (decltype(_wglDeleteContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglDeleteContext");
    _wglCopyContext     = (decltype(_wglCopyContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCopyContext");

    if (not _wglMakeCurrent
        or not _wglCreateContext
        or not _wglGetProcAddress
        or not _wglDeleteContext) {
        Error("Failed to load required WGL function.");
    }


    GLCTX dummy_context = nullptr;

    dummy_context = _wglCreateContext(m_MainHDC);
    if (!dummy_context) {
        Error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!_wglMakeCurrent(m_MainHDC, dummy_context)) {
        Error("Failed to activate dummy OpenGL rendering context.");
    }

    wglCreateContextAttribsARB  = (decltype(wglCreateContextAttribsARB))rsgl("wglCreateContextAttribsARB");
    wglGetExtensionsStringARB   = (decltype(wglGetExtensionsStringARB))rsgl("wglGetExtensionsStringARB");

    if (!wglGetExtensionsStringARB or !wglCreateContextAttribsARB) {
        Error("Failed to load required WGL extensions.");
    }

    int gl_attribs[] = { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 4,
        WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, 0x8252, // WGL_LOSE_CONTEXT_ON_RESET_ARB
    #ifdef DEBUG
        WGL_CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
    #endif
        WGL_CONTEXT_PROFILE_MASK_ARB, 0x00000001, // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
        0,
    };


    GLCTX opengl_context = nullptr;
    if (nullptr == (opengl_context = wglCreateContextAttribsARB(m_MainHDC, nullptr, gl_attribs))) {
        _wglDeleteContext(dummy_context);
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            Error("Unsupported GL Version {}.{}", gl_attribs[1], gl_attribs[3]);
        }
        Error("Failed to create the final rendering context!");
    }

    _wglMakeCurrent(m_MainHDC, opengl_context); // conseder to make it curent in window.hpp maybe
    _wglDeleteContext(dummy_context);

    m_Context =  opengl_context;

    glGetError = reinterpret_cast<decltype(glGetError)>(rsgl("glGetError"));

    #ifdef DEBUG
    #   define RESOLVEGL(type, name)\
        name = Function<type>{};\
        name.m_Func = reinterpret_cast<type>(rsgl(#name));\
        name.m_Name = #name;
    #else
    #   define RESOLVEGL(type, name)\
        name = reinterpret_cast<type>(rsgl(#name))
    #endif

	GLFUNCS(RESOLVEGL)
}


#elif defined(__linux__)

auto OpenGL::init_opengl_linux(Window window) -> void
{
    XWindowAttributes win_attr{};
    if (!XGetWindowAttributes(m_MainHDC, window, &win_attr)) {
        Error("Failed to get window attributes.");
    }
    
    XVisualInfo visual_info_template{};
    visual_info_template.visualid = win_attr.visual->visualid;
    int num_visuals = 0;
    XVisualInfo* visual_info_list = XGetVisualInfo(m_MainHDC, VisualIDMask, &visual_info_template, &num_visuals);
    if (num_visuals == 0) {
        Error("No matching XVisualInfo found.");
    }
    
    m_Context = glXCreateContext(m_MainHDC, visual_info_list, nullptr, GL_TRUE);
        
    if (!glXMakeCurrent(m_MainHDC, window, m_Context)) {
        Error("Failed to make context current.");
    }

}
#endif //_WIN32

OpenGL::OpenGL(WindHandl window, HDC_D hdcd)
    : m_Context(nullptr)
    , m_Major(0)
    , m_Minor(0)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(false)
{
    #ifdef _WIN32
    if(hdcd == HDC_D{})
        m_MainHDC = GetDC(window);
    else
        m_MainHDC = hdcd;

    
    if( m_MainHDC == nullptr){
        Error("HDC not valid");
    }
    init_opengl_win32();
    
    #elif defined(__linux__)
    if(hdcd == HDC_D{})
        Error("invalid HDC_D");
    else
        m_MainHDC = hdcd;

    if( m_MainHDC == nullptr){
        Error("HDC not valid");
    }

    init_opengl_linux(window);
    #endif //_WIN32

    glGetIntegerv(GL_MAJOR_VERSION, &m_Major);
    glGetIntegerv(GL_MINOR_VERSION, &m_Minor);
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    m_Debug = !!(flags & GL_CONTEXT_FLAG_DEBUG_BIT);
    
    static bool isInitialized = false;
    if (!isInitialized) {
        m_Vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        m_Renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        #ifdef _WIN32
        m_Extensions = split(reinterpret_cast<const char*>(wglGetExtensionsStringARB(m_MainHDC)), " ");
        #endif //_WIN32
        
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


    #ifdef DEBUG
    if( m_Major >= 4 and m_Minor >= 3 and m_Debug){
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    #endif

    Info("GL Version : {}.{}", m_Major, m_Minor);
    Info("GLSL Version Supported : {}", to_string(m_GlslVersions));
    Info("GL Vendor : {}", m_Vendor);
    Info("GL Renderer : {}", m_Renderer);
    Info("GL Exts : {}", to_string(m_Extensions));
    Info("Max Texture Units : {}", m_MaxTextureUnits);
    Info("Debug : {}", m_Debug ? "On" : "Off");
}

OpenGL::OpenGL(const OpenGL &other)
    : m_Context(nullptr)
    , m_MainHDC(other.m_MainHDC)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(other.m_Debug)
{
    #if _WIN32
    auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) Error("couldn't Copy Opengl Context");
    #elif defined(__linux__)
    #warning "not impl for linux"
    #endif //_WIN32
}

auto OpenGL::operator=(const OpenGL &other) -> OpenGL
{
    if(*this != other){
        this->m_Context = nullptr;
        this->m_MainHDC = other.m_MainHDC;    
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = std::time(nullptr);
        this->m_Debug = other.m_Debug;

        #if _WIN32
        auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        if(tst != TRUE) Error("couldn't Copy Opengl Context");
        #elif defined(__linux__)
        #warning "not impl for linux"
        #endif //_WIN32
    }
    return *this;
}

OpenGL::OpenGL(OpenGL &&other) noexcept
    : m_Context(other.m_Context)
    , m_MainHDC(other.m_MainHDC)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(other.m_CreationTime)
    , m_Debug(other.m_Debug)
{
    other.m_Context = nullptr;
    other.m_MainHDC = nullptr;
    other.m_Major = 0;
    other.m_Minor = 0;
    other.m_CreationTime = 0;
    other.m_Debug = false;
}

auto OpenGL::operator=(OpenGL &&other) noexcept -> OpenGL
{
    if(*this != other){
        this->m_Context = other.m_Context;
        this->m_MainHDC = other.m_MainHDC;
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = other.m_CreationTime;
        this->m_Debug = other.m_Debug;
        
        other.m_Context = nullptr;
        other.m_MainHDC = nullptr;
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
    #ifdef _WIN32
    _wglMakeCurrent(nullptr, nullptr);
    _wglDeleteContext(m_Context);
    #elif defined(__linux__)
    glXDestroyContext(m_MainHDC, m_Context);
    #endif //_WIN32
}

auto OpenGL::DrawContext() const -> HDC_D
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
    return m_Context != nullptr;
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
