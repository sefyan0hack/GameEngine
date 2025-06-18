#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>

namespace gl {

static std::mutex g_GetProcMutex;

/* dead code 
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
*/

auto __GetProcAddress(const char* module, const char* name) -> void* {
    std::lock_guard<std::mutex> lock(g_GetProcMutex);

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
    #elif defined(WEB_PLT)
    void* address = (void*)emscripten_webgl_get_proc_address(name);
    #endif

    return address;
}

auto resolve_opengl_fn(const char* name) -> void* {

    #if defined(WINDOWS_PLT)
    void *address = (void *)wglGetProcAddress(name);

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
    #elif defined(WEB_PLT)
    void* address = __GetProcAddress(OPENGL_MODULE_NAME, name);
    if (address != nullptr) {
        Info("from LIB:`{}`: load function `{}` at : {}", OPENGL_MODULE_NAME, name, address);
    } else {
        Info("Couldn't load {} function `{}`", OPENGL_MODULE_NAME, name);
        // Error("Couldn't load WebGL function `{}`", name);
    }
    #endif

    return address;
}

#if defined(WINDOWS_PLT)

auto OpenGL::init_opengl_win32() -> void
{
    CheckThread();

    PIXELFORMATDESCRIPTOR pfd {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    auto pixel_format = ChoosePixelFormat(m_DrawContext, &pfd);
    if (!pixel_format) {
        Error("Failed to find a suitable pixel format. : {}", GetLastError());
    }
    if (!SetPixelFormat(m_DrawContext, pixel_format, &pfd)) {
        Error("Failed to set the pixel format. : {}", GetLastError());
    }

    GLCTX dummy_context = nullptr;

    dummy_context = wglCreateContext(m_DrawContext);
    if (!dummy_context) {
        Error("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    if (!wglMakeCurrent(m_DrawContext, dummy_context)) {
        Error("Failed to activate dummy OpenGL rendering context. : {}", GetLastError());
    }

    wglCreateContextAttribsARB  = (decltype(wglCreateContextAttribsARB))wglGetProcAddress("wglCreateContextAttribsARB");
    wglGetExtensionsStringARB   = (decltype(wglGetExtensionsStringARB))wglGetProcAddress("wglGetExtensionsStringARB");

    if (!wglGetExtensionsStringARB || !wglCreateContextAttribsARB) {
        Error("Failed to load required WGL extensions. : {}", GetLastError());
    }

    int32_t gl_attribs[] = { 
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
        wglDeleteContext(dummy_context);
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            Error("Unsupported GL Version {}.{}", gl_attribs[1], gl_attribs[3]);
        }
        Error("Failed to create the final rendering context!");
    }

    wglDeleteContext(dummy_context);

    m_Context =  opengl_context;
}


#elif defined(LINUX_PLT)

auto OpenGL::init_opengl_linux() -> void
{
    CheckThread();

    static int32_t visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_DEPTH_SIZE,     24,
        0
    };
    
    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(m_DrawContext, DefaultScreen(m_DrawContext), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        Error("Failed to get framebuffer config.");
    }

    XVisualInfo* visInfo = glXGetVisualFromFBConfig(m_DrawContext, fbc[0]);
    if (!visInfo) {
        XFree(fbc);
        Error("Failed to get visual info.");
    }

    int32_t contextAttribs[] = {
        CONTEXT_MAJOR_VERSION_ARB, 4,
        CONTEXT_MINOR_VERSION_ARB, 4,
        CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB, 0x8252, // GLX_LOSE_CONTEXT_ON_RESET_ARB
        #ifdef DEBUG
        CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // CONTEXT_DEBUG_BIT_ARB | CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
        #endif
        CONTEXT_PROFILE_MASK_ARB,  0x00000001,
        0
    };

    glXCreateContextAttribsARB = (decltype(glXCreateContextAttribsARB))glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    m_Context = glXCreateContextAttribsARB(m_DrawContext, fbc[0], nullptr, True, contextAttribs);
    XFree(fbc);
    XFree(visInfo);

    if (!m_Context) {
        Error("Failed to create GLX context.");
    }
}
#elif defined(WEB_PLT)
auto OpenGL::init_opengl_web() -> void
{
    CheckThread();

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    
    attrs.alpha = EM_TRUE;
    attrs.depth = EM_TRUE;
    attrs.stencil = EM_TRUE;
    attrs.antialias = EM_TRUE;
    attrs.premultipliedAlpha = EM_FALSE;
    attrs.preserveDrawingBuffer = EM_FALSE;
    attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
    attrs.failIfMajorPerformanceCaveat = EM_FALSE;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.enableExtensionsByDefault = EM_TRUE;

    m_Context = emscripten_webgl_create_context("#canvas", &attrs);
    
    if (m_Context <= 0) {
        attrs.majorVersion = 1;
        m_Context = emscripten_webgl_create_context("#canvas", &attrs);
        
        if (m_Context <= 0) {
            Error("Failed to create WebGL context: error {}", static_cast<int32_t>(m_Context));
            return;
        }
    }
}

#endif

OpenGL::OpenGL([[maybe_unused]] WindHandl window, HDC_D drawContext)
    : m_Context(GLCTX{})
    , m_DrawContext(drawContext)
    , m_Major(0)
    , m_Minor(0)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(false)
    , m_ThreadId(std::hash<std::thread::id>{}(std::this_thread::get_id()))
{
    #if defined(WINDOWS_PLT)
    init_opengl_win32();
    if (!wglMakeCurrent(m_DrawContext, m_Context)){
		Error("Failed to make context current.");
	}
    #elif defined(LINUX_PLT)
    init_opengl_linux();
    if (!glXMakeCurrent(m_DrawContext, window, m_Context)) {
        Error("Failed to make context current.");
    }
    #elif defined(WEB_PLT)
    init_opengl_web();
    if (emscripten_webgl_make_context_current(m_Context) != EMSCRIPTEN_RESULT_SUCCESS) {
        Error("Failed to make WebGL context current.");
    }
    #endif

    glGetError = reinterpret_cast<PFNGLGETERRORPROC>(resolve_opengl_fn("glGetError"));

    #ifdef DEBUG
    #   define RESOLVEGL(type, name)\
        OpenGL::name = Function<type>{};\
        OpenGL::name.m_Func  = reinterpret_cast<type>(resolve_opengl_fn("gl"#name));\
        OpenGL::name.m_After = []([[maybe_unused]] std::string info) { GLenum err = glGetError(); if(err != GL_NO_ERROR) Info("[{}] {}", GL_ERR_to_string(err), info); };\
        OpenGL::name.m_Name  = "gl"#name
    #else
    #   define RESOLVEGL(type, name)\
        OpenGL::name = reinterpret_cast<type>(resolve_opengl_fn("gl"#name))
    #endif

	GLFUNCS(RESOLVEGL)

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);
    
    auto vendor = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(gl::GetString(GL_RENDERER));
    m_Vendor = vendor ? vendor : "unknown";
    m_Renderer = renderer ? renderer : "unknown";

    #if defined(WINDOWS_PLT)
    auto exts = reinterpret_cast<const char*>(wglGetExtensionsStringARB(m_DrawContext));
    #elif defined(LINUX_PLT)
    auto exts = reinterpret_cast<const char*>(glXQueryExtensionsString(m_DrawContext, DefaultScreen(m_DrawContext)));
    #elif defined(WEB_PLT)
    auto exts = emscripten_webgl_get_supported_extensions();
    #endif

    m_Extensions = exts ? split(exts, " ") : decltype(m_Extensions){} ;
    
    if constexpr (sys::Target != sys::Target::Web){
        GLint flags = 0;
        gl::GetIntegerv(GL_CONTEXT_FLAGS, &flags);
        m_Debug = !!(flags & GL_CONTEXT_FLAG_DEBUG_BIT);

        GLint nGlslv = 0;
        gl::GetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &nGlslv);

        if(static_cast<size_t>(nGlslv) != m_GlslVersions.size()){
            for(GLint i = 0; i < nGlslv; i++){
                auto r = reinterpret_cast<const char*>(gl::GetStringi(GL_SHADING_LANGUAGE_VERSION, static_cast<GLuint>(i)));
                if(r) m_GlslVersions.push_back(r);

            }
        }
    }else{
        m_GlslVersions.push_back("300 es");
    }
    gl::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_MaxTextureUnits);

    gl::Enable(GL_DEPTH_TEST);
    gl::DepthFunc(GL_LESS);

    // gl::Enable(GL_BLEND);
    // gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
    if constexpr (sys::Target != sys::Target::Web){
        gl::Enable(GL_LINE_SMOOTH);
        gl::Enable(GL_MULTISAMPLE);
        gl::Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }

    /* dead code 
    #if defined(DEBUG) && !defined(WEB_PLT)
    if( m_Major >= 4 && m_Minor >= 3 && m_Debug)
    {
        gl::Enable(GL_DEBUG_OUTPUT);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        gl::DebugMessageCallback(GLDebugMessageCallback, nullptr);
        gl::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    #endif
    */

    Info("Platform : {}, Arch : {}", sys::TargetName, sys::ArchName);
    Info("GL Thread id : {}", m_ThreadId);
    Info("GL Version : {}.{}", m_Major, m_Minor);
    Info("GLSL Version Supported : {}", to_string(m_GlslVersions));
    Info("GL Vendor : {}", m_Vendor);
    Info("GL Renderer : {}", m_Renderer);
    Info("GL Exts : {}", to_string(m_Extensions));
    Info("Max Texture Units : {}", m_MaxTextureUnits);
    Info("Debug : {}", m_Debug ? "On" : "Off");
}

OpenGL::OpenGL(const OpenGL &other)
    : m_Context(GLCTX{})
    , m_DrawContext(other.m_DrawContext)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
    , m_Debug(other.m_Debug)
    , m_ThreadId(other.m_ThreadId)
{
    #if defined(WINDOWS_PLT)
    auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) Error("couldn't Copy Opengl Context");
    #elif defined(LINUX_PLT)
    glXCopyContext(this->m_DrawContext, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    // no error check for now  ` X11 ` Shit
    #endif
}

auto OpenGL::operator=(const OpenGL &other) -> OpenGL&
{
    if(*this != other){
        this->m_Context = GLCTX{};
        this->m_DrawContext = other.m_DrawContext;    
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = std::time(nullptr);
        this->m_Debug = other.m_Debug;

        #if defined(WINDOWS_PLT)
        auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        if(tst != TRUE) Error("couldn't Copy Opengl Context");
        #elif defined(LINUX_PLT)
        glXCopyContext(this->m_DrawContext, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        // no error check for now  ` X11 ` Shit
        #endif //_WIN32
    }
    return *this;
}

OpenGL::OpenGL(OpenGL &&other) noexcept
    : m_Context(std::exchange(other.m_Context, GLCTX{}))
    , m_DrawContext(std::exchange(other.m_DrawContext, nullptr))
    , m_Major(std::exchange(other.m_Major, 0))
    , m_Minor(std::exchange(other.m_Minor, 0))
    , m_CreationTime(std::exchange(other.m_CreationTime, 0))
    , m_Debug(std::exchange(other.m_Debug, false))
    , m_ThreadId(std::exchange(other.m_ThreadId, 0))
{
}

auto OpenGL::operator=(OpenGL &&other) noexcept -> OpenGL&
{
    if(*this != other){
        this->m_Context = std::exchange(other.m_Context, GLCTX{});
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
    CheckThread();
    return isValid();
}

OpenGL::~OpenGL()
{
    CheckThread();
    #if defined(WINDOWS_PLT)
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_Context);
    #elif defined(LINUX_PLT)
    glXMakeCurrent(m_DrawContext, Window{},  GLCTX{});
    glXDestroyContext(m_DrawContext, m_Context);
    #elif defined(WEB_PLT)
    if (m_Context > 0) {
        emscripten_webgl_destroy_context(m_Context);
    }
    #endif
}

auto OpenGL::Context() const -> GLCTX
{
    CheckThread();
    return m_Context;
}

auto OpenGL::DrawContext() const -> HDC_D
{
    CheckThread();
    return m_DrawContext;
}

auto OpenGL::MajorV() const -> GLint
{
    CheckThread();
    return m_Major;
}
auto OpenGL::MinorV() const -> GLint
{
    CheckThread();
    return m_Minor;
}

auto OpenGL::isValid() const -> bool
{
    CheckThread();
    return m_Context != GLCTX{};
}

auto OpenGL::CreationTime() const -> std::time_t
{
    CheckThread();
    return m_CreationTime;
}

auto OpenGL::isDebugable() const -> bool
{
    CheckThread();
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

auto OpenGL::ThreadId() const -> size_t
{
    CheckThread();
    return m_ThreadId;
}

auto OpenGL::CheckThread() const -> void
{
    if constexpr (sys::Target != sys::Target::Web){

        auto id = std::hash<std::thread::id>{}(std::this_thread::get_id());
        if ( id != m_ThreadId ) {
            Error("OpenGL context used in wrong thread! . Expected id: {} Vs Geted: {}", m_ThreadId, id);
        }
    }
}

} //namespace gl