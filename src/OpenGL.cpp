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

auto __GetProcAddress(const char* module, const char* name) -> void* {
    #if defined(WINDOWS_PLT)
    auto lib = GetModuleHandleA(module);

    if(lib == nullptr){
        lib = LoadLibraryA(module);
        
        if(lib == nullptr){
            Error("Couldnt load lib {} reason: {}", module, GetLastError());
            return nullptr;
        }
    }

    void *address = (void *)GetProcAddress(lib, name);
    #elif defined(LINUX_PLT)
    void* lib = dlopen(module, RTLD_LAZY | RTLD_NOLOAD);
    
    if(lib == nullptr) {
        lib = dlopen(module, RTLD_LAZY);
        if(lib == nullptr) {
            Error("Couldn't load lib {} reason: {}", module, dlerror());
            return nullptr;
        }
    }

    dlerror();
    void* address = (void *)dlsym(lib, name);
    #endif

    if(address != nullptr)
    {
        Info("from LIB:`{}`: load function `{}` at : {}", module, name, address);
        return address;
    }else{
        Error("Couldnt load function `{}` from module : {}", name, module);
        return nullptr;
    }
}

auto resolve_opengl_fn(const char* name) -> void* {
    #if defined(WINDOWS_PLT)
    void *address = (void *)_wglGetProcAddress(name);

    if(address == nullptr
    || address == reinterpret_cast<void*>(0x1)
    || address == reinterpret_cast<void*>(0x2)
    || address == reinterpret_cast<void*>(0x3)
    || address == reinterpret_cast<void*>(-1))
    {
        address = __GetProcAddress(OPENGL_MODULE_NAME, name);
        if(address == nullptr){
            Error("Couldnt load opengl function `{}` reason: {}", name, GetLastError());
        }
    }else{
        Info("from LIB:`{}`: load function `{}` at : {}", OPENGL_MODULE_NAME, name, address);
    }

    #elif defined(LINUX_PLT)
    void *address = (void *)__GetProcAddress(OPENGL_MODULE_NAME, name);
    if(address == nullptr){
        Error("Couldnt load opengl function `{}`", name);
    }else{
        Info("from LIB:`opengl32`: load function `{}` at : {}", name, address);
    }
    #endif

    return address;
}

#if defined(WINDOWS_PLT)

auto OpenGL::init_opengl_win32() -> void
{
    _wglMakeCurrent        = (decltype(_wglMakeCurrent))__GetProcAddress(OPENGL_MODULE_NAME, "wglMakeCurrent");
    _wglCreateContext      = (decltype(_wglCreateContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCreateContext");
    _wglGetProcAddress     = (decltype(_wglGetProcAddress))__GetProcAddress(OPENGL_MODULE_NAME, "wglGetProcAddress");
    _wglDeleteContext      = (decltype(_wglDeleteContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglDeleteContext");
    _wglCopyContext        = (decltype(_wglCopyContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCopyContext");
    _wglCreateLayerContext = (decltype(_wglCreateLayerContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglCreateLayerContext");
    _wglGetCurrentContext  = (decltype(_wglGetCurrentContext))__GetProcAddress(OPENGL_MODULE_NAME, "wglGetCurrentContext");
    _wglGetCurrentDC       = (decltype(_wglGetCurrentDC))__GetProcAddress(OPENGL_MODULE_NAME, "wglGetCurrentDC");
    _wglShareLists         = (decltype(_wglShareLists))__GetProcAddress(OPENGL_MODULE_NAME, "wglShareLists");
    _wglUseFontBitmapsA    = (decltype(_wglUseFontBitmapsA))__GetProcAddress(OPENGL_MODULE_NAME, "wglUseFontBitmapsA");
    _wglUseFontBitmapsW    = (decltype(_wglUseFontBitmapsW))__GetProcAddress(OPENGL_MODULE_NAME, "wglUseFontBitmapsW");

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

    auto pixel_format = ChoosePixelFormat(m_DrawContext, &pfd);
    if (!pixel_format) {
        Error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(m_DrawContext, pixel_format, &pfd)) {
        Error("Failed to set the pixel format.");
    }

    GLCTX dummy_context = nullptr;

    dummy_context = _wglCreateContext(m_DrawContext);
    if (!dummy_context) {
        Error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!_wglMakeCurrent(m_DrawContext, dummy_context)) {
        Error("Failed to activate dummy OpenGL rendering context.");
    }

    wglCreateContextAttribsARB  = (decltype(wglCreateContextAttribsARB))resolve_opengl_fn("wglCreateContextAttribsARB");
    wglGetExtensionsStringARB   = (decltype(wglGetExtensionsStringARB))resolve_opengl_fn("wglGetExtensionsStringARB");

    if (!wglGetExtensionsStringARB || !wglCreateContextAttribsARB) {
        Error("Failed to load required WGL extensions.");
    }

    int gl_attribs[] = { 
        CONTEXT_MAJOR_VERSION_ARB, 4,
        CONTEXT_MINOR_VERSION_ARB, 4,
        CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, 0x8252, // WGL_LOSE_CONTEXT_ON_RESET_ARB
    #ifdef DEBUG
        CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // CONTEXT_DEBUG_BIT_ARB | CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
    #endif
        CONTEXT_PROFILE_MASK_ARB, 0x00000001, // CONTEXT_CORE_PROFILE_BIT_ARB
        0,
    };


    GLCTX opengl_context = nullptr;
    if (nullptr == (opengl_context = wglCreateContextAttribsARB(m_DrawContext, nullptr, gl_attribs))) {
        _wglDeleteContext(dummy_context);
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            Error("Unsupported GL Version {}.{}", gl_attribs[1], gl_attribs[3]);
        }
        Error("Failed to create the final rendering context!");
    }

    _wglDeleteContext(dummy_context);

    m_Context =  opengl_context;
}


#elif defined(LINUX_PLT)

auto OpenGL::init_opengl_linux() -> void
{
    _glXCreateContext         = (decltype(_glXCreateContext))__GetProcAddress(OPENGL_MODULE_NAME, "glXCreateContext");
    _glXMakeCurrent           = (decltype(_glXMakeCurrent))__GetProcAddress(OPENGL_MODULE_NAME, "glXMakeCurrent");
    _glXDestroyContext        = (decltype(_glXDestroyContext))__GetProcAddress(OPENGL_MODULE_NAME, "glXDestroyContext");
    _glXGetProcAddress        = (decltype(_glXGetProcAddress))__GetProcAddress(OPENGL_MODULE_NAME, "glXGetProcAddress");
    _glXChooseFBConfig        = (decltype(_glXChooseFBConfig))__GetProcAddress(OPENGL_MODULE_NAME, "glXChooseFBConfig");
    _glXGetVisualFromFBConfig = (decltype(_glXGetVisualFromFBConfig))__GetProcAddress(OPENGL_MODULE_NAME, "glXGetVisualFromFBConfig");
    _glXSwapBuffers           = (decltype(_glXSwapBuffers))__GetProcAddress(OPENGL_MODULE_NAME, "glXSwapBuffers");
    _glXQueryExtensionsString = (decltype(_glXQueryExtensionsString))__GetProcAddress(OPENGL_MODULE_NAME, "glXQueryExtensionsString");
    _glXCopyContext           = (decltype(_glXCopyContext))__GetProcAddress(OPENGL_MODULE_NAME, "glXCopyContext");

    static int visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DRAWABLE_TYPE, 0x00000001,
        GLX_RENDER_TYPE,   0x00000001,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_DEPTH_SIZE,     24,
        0
    };

    int fbcount;
    GLXFBConfig* fbc = _glXChooseFBConfig(m_DrawContext, DefaultScreen(m_DrawContext), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        Error("Failed to get framebuffer config.");
    }

    XVisualInfo* visInfo = _glXGetVisualFromFBConfig(m_DrawContext, fbc[0]);
    if (!visInfo) {
        XFree(fbc);
        Error("Failed to get visual info.");
    }

    int contextAttribs[] = {
        CONTEXT_MAJOR_VERSION_ARB, 4,
        CONTEXT_MINOR_VERSION_ARB, 4,
        CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, 0x8252, // GLX_LOSE_CONTEXT_ON_RESET_ARB
        #ifdef DEBUG
        CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // CONTEXT_DEBUG_BIT_ARB | CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
        #endif
        CONTEXT_PROFILE_MASK_ARB,  0x00000001,
        0
    };

    _glXCreateContextAttribsARB = (decltype(_glXCreateContextAttribsARB))_glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    m_Context = _glXCreateContextAttribsARB(m_DrawContext, fbc[0], nullptr, True, contextAttribs);
    XFree(fbc);
    XFree(visInfo);

    if (!m_Context) {
        Error("Failed to create GLX context.");
    }
}
#endif

OpenGL::OpenGL([[maybe_unused]] WindHandl window, HDC_D drawContext)
    : m_Context(nullptr)
    , m_DrawContext(drawContext)
    , m_Major(0)
    , m_Minor(0)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(false)
{
    #if defined(WINDOWS_PLT)
    init_opengl_win32();
    if (!_wglMakeCurrent(m_DrawContext, m_Context)){
		Error("Failed to make context current.");
	}
    #elif defined(LINUX_PLT)
    init_opengl_linux();
    if (!_glXMakeCurrent(m_DrawContext, window, m_Context)) {
        Error("Failed to make context current.");
    }
    #endif //_WIN32


    glGetError = reinterpret_cast<decltype(glGetError)>(resolve_opengl_fn("glGetError"));

    #ifdef DEBUG
    #   define RESOLVEGL(type, name)\
        name = Function<type>{};\
        name.m_Func = reinterpret_cast<type>(resolve_opengl_fn(#name));\
        name.m_Name = #name;
    #else
    #   define RESOLVEGL(type, name)\
        name = reinterpret_cast<type>(resolve_opengl_fn(#name))
    #endif

	GLFUNCS(RESOLVEGL)

    glGetIntegerv(GL_MAJOR_VERSION, &m_Major);
    glGetIntegerv(GL_MINOR_VERSION, &m_Minor);
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    m_Debug = !!(flags & GL_CONTEXT_FLAG_DEBUG_BIT);
    
    auto vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    m_Vendor = vendor ? vendor : "unknown";
    m_Renderer = renderer ? renderer : "unknown";

    #if defined(WINDOWS_PLT)
    auto exts = reinterpret_cast<const char*>(wglGetExtensionsStringARB(m_DrawContext));
    #elif defined(LINUX_PLT)
    auto exts = reinterpret_cast<const char*>(_glXQueryExtensionsString(m_DrawContext, DefaultScreen(m_DrawContext)));
    #endif

    m_Extensions = exts ? split(exts, " ") : decltype(m_Extensions){} ;
        
    GLint nGlslv = 0;
    glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &nGlslv);

    for(GLint i = 0; i < nGlslv; i++){
        auto r = reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, static_cast<GLuint>(i)));
        if(r) m_GlslVersions.push_back(r);
    }

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_MaxTextureUnits);
    

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
    if( m_Major >= 4 && m_Minor >= 3 && m_Debug){
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
    , m_DrawContext(other.m_DrawContext)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(other.m_Debug)
{
    #if defined(WINDOWS_PLT)
    auto tst = _wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) Error("couldn't Copy Opengl Context");
    #elif defined(LINUX_PLT)
    _glXCopyContext(this->m_DrawContext, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    // no error check for now  ` X11 ` Shit
    #endif
}

auto OpenGL::operator=(const OpenGL &other) -> OpenGL
{
    if(*this != other){
        this->m_Context = nullptr;
        this->m_DrawContext = other.m_DrawContext;    
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = std::time(nullptr);
        this->m_Debug = other.m_Debug;

        #if defined(WINDOWS_PLT)
        auto tst = _wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        if(tst != TRUE) Error("couldn't Copy Opengl Context");
        #elif defined(LINUX_PLT)
        _glXCopyContext(this->m_DrawContext, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        // no error check for now  ` X11 ` Shit
        #endif //_WIN32
    }
    return *this;
}

OpenGL::OpenGL(OpenGL &&other) noexcept
    : m_Context(std::exchange(other.m_Context, nullptr))
    , m_DrawContext(std::exchange(other.m_DrawContext, nullptr))
    , m_Major(std::exchange(other.m_Major, 0))
    , m_Minor(std::exchange(other.m_Minor, 0))
    , m_CreationTime(std::exchange(other.m_CreationTime, 0))
    , m_Debug(std::exchange(other.m_Debug, false))
{
}

auto OpenGL::operator=(OpenGL &&other) noexcept -> OpenGL
{
    if(*this != other){
        this->m_Context = std::exchange(other.m_Context, nullptr);
        this->m_DrawContext = std::exchange(other.m_DrawContext, nullptr);
        this->m_Major = std::exchange(other.m_Major, 0);
        this->m_Minor = std::exchange(other.m_Minor, 0);
        this->m_CreationTime = std::exchange(other.m_CreationTime, 0);
        this->m_Debug = std::exchange(other.m_Debug, false);
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
    #if defined(WINDOWS_PLT)
    _wglMakeCurrent(nullptr, nullptr);
    _wglDeleteContext(m_Context);
    #elif defined(LINUX_PLT)
    _glXMakeCurrent(m_DrawContext, Window{},  GLCTX{});
    _glXDestroyContext(m_DrawContext, m_Context);
    #endif
}

auto OpenGL::Context() const -> GLCTX
{
    return m_Context;
}

auto OpenGL::DrawContext() const -> HDC_D
{
    return m_DrawContext;
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
