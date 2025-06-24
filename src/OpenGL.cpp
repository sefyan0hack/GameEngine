#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>

namespace gl {

#if defined(WINDOWS_PLT)

auto OpenGL::init_opengl_win32() -> void
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        static_cast<BYTE>(ChannelBits * 3),  // RGB bits
        static_cast<BYTE>(AlphaBits),         // Alpha bits
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        static_cast<BYTE>(DepthBufferBits),   // Depth buffer
        static_cast<BYTE>(StencilBufferBits), // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0, 0, 0
    };
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

    wglGetExtensionsStringARB   = (decltype(wglGetExtensionsStringARB))wglGetProcAddress("wglGetExtensionsStringARB");
    wglCreateContextAttribsARB  = (decltype(wglCreateContextAttribsARB))wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglGetExtensionsStringARB){
        Error("Failed to load wglGetExtensionsStringARB. : {}", GetLastError());
    }

    if(!wglCreateContextAttribsARB) {
        Error("Failed to load wglCreateContextAttribsARB. : {}", GetLastError());
    }

    int32_t gl_attribs[] = { 
        CONTEXT_MAJOR_VERSION_ARB, GLMajorVersion,
        CONTEXT_MINOR_VERSION_ARB, GLMinorVersion,
    #ifdef DEBUG
        CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // CONTEXT_DEBUG_BIT_ARB | CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
    #endif
        0,
    };


    GLCTX opengl_context = nullptr;
    if (nullptr == (opengl_context = wglCreateContextAttribsARB(m_DrawContext, nullptr, gl_attribs))) {
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            Error("Unsupported GL Version {}.{}", GLMajorVersion, GLMinorVersion);
        }
        Error("Failed to create the final rendering context!");
    }

    wglDeleteContext(dummy_context);

    m_Context =  opengl_context;
}


#elif defined(LINUX_PLT)

auto OpenGL::init_opengl_linux() -> void
{

    static int32_t visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       ChannelBits,
        GLX_GREEN_SIZE,     ChannelBits,
        GLX_BLUE_SIZE,      ChannelBits,
        GLX_ALPHA_SIZE,     AlphaBits,
        GLX_DEPTH_SIZE,     DepthBufferBits,
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
        CONTEXT_MAJOR_VERSION_ARB, GLMajorVersion,
        CONTEXT_MINOR_VERSION_ARB, GLMinorVersion,
        #ifdef DEBUG
        CONTEXT_FLAGS_ARB, 0x0001 | 0x0002,  // CONTEXT_DEBUG_BIT_ARB | CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
        #endif
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

    #ifdef DEBUG
    #   define RESOLVEGL(name)\
        OpenGL::name = Function<decltype(&gl##name)>{};\
        OpenGL::name.m_Func  = reinterpret_cast<decltype(&gl##name)>(gl::GetProcAddress("gl"#name));\
        OpenGL::name.m_After = []([[maybe_unused]] std::string info) { GLenum err = glGetError(); if(err != GL_NO_ERROR) Info("[{}] {}", GL_ERR_to_string(err), info); };\
        OpenGL::name.m_Name  = "gl"#name
    #else
    #   define RESOLVEGL(name)\
        OpenGL::name = reinterpret_cast<decltype(&gl##name)>(gl::GetProcAddress("gl"#name))
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
    
    gl::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_MaxTextureUnits);

    gl::Enable(GL_DEPTH_TEST);
    gl::DepthFunc(GL_LESS);


    if constexpr (sys::Target != sys::Target::Web){
        gl::Enable(GL_LINE_SMOOTH);
        gl::Enable(GL_MULTISAMPLE);
        gl::Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }

    Info("Platform : {}, Arch : {}", sys::TargetName, sys::ArchName);
    Info("GL Version : Wanted:({}.{}) -> Got:({}.{})", GLMajorVersion, GLMinorVersion, m_Major, m_Minor);
    Info("GL Vendor : {}", m_Vendor);
    Info("GL Renderer : {}", m_Renderer);
    Info("GL Exts : {}", to_string(m_Extensions));
    Info("Max Texture Units : {}", m_MaxTextureUnits);
}

OpenGL::OpenGL(const OpenGL &other)
    : m_Context(GLCTX{})
    , m_DrawContext(other.m_DrawContext)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
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
    return m_Context != GLCTX{};
}

auto OpenGL::CreationTime() const -> std::time_t
{
    return m_CreationTime;
}


auto OpenGL::Vendor() -> std::string
{
    return m_Vendor;
}
auto OpenGL::Renderer() -> std::string
{
    return m_Renderer;
}


auto OpenGL::Extensions() -> std::vector<std::string>
{
    return m_Extensions;
}

auto OpenGL::MaxTextureUnits() -> GLint
{
    return m_MaxTextureUnits;
}

auto GetProcAddress(const char* name) -> void* {
    void *address = nullptr;

    #if defined(WINDOWS_PLT)
    address = reinterpret_cast<void*>(wglGetProcAddress(name));

    if(address == nullptr
    || address == reinterpret_cast<void*>(0x1)
    || address == reinterpret_cast<void*>(0x2)
    || address == reinterpret_cast<void*>(0x3)
    || address == reinterpret_cast<void*>(-1))
    {
        address = ::GetProcAddress(OPENGL_MODULE_NAME, name);
    }

    #elif defined(LINUX_PLT) || defined(WEB_PLT)
    address = ::GetProcAddress(OPENGL_MODULE_NAME, name);
    #endif

    if (address != nullptr) {
        Info("from LIB:`{}`: load function `{}` at : {}", OPENGL_MODULE_NAME, name, address);
    } else {
        Error("Couldn't load {} function `{}`", OPENGL_MODULE_NAME, name);
    }

    return address;
}


auto gl::OpenGL::DummyCtx() -> GLCTX
{
    GLCTX dummy_context{};

    #if defined(WINDOWS_PLT)
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const char* className = "DummyGLWindow";

    WNDCLASS existingClass;
    if (!GetClassInfo(hInstance, className, &existingClass)) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;

        if (!RegisterClass(&wc)) {
            DWORD error = GetLastError();
            if (error != ERROR_CLASS_ALREADY_EXISTS) {
                Error("Can't register Window class: {}", error);
            }
        }
    }

    HWND hwnd = CreateWindow(
        className, "Dummy OpenGL",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!hwnd){
        Error("Can't Create Window : {}", GetLastError());
    }

    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = ChannelBits + ChannelBits + ChannelBits;//rgb
    pfd.cAlphaBits = AlphaBits; //a
    pfd.cDepthBits = DepthBufferBits;
    pfd.cStencilBits = StencilBufferBits;
    pfd.iLayerType = PFD_MAIN_PLANE;

    auto pixel_format = ChoosePixelFormat(hdc, &pfd);
    if (!pixel_format) {
        Error("Failed to find a suitable pixel format. : {}", GetLastError());
    }
    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        Error("Failed to set the pixel format. : {}", GetLastError());
    }

    dummy_context = wglCreateContext(hdc);
    if (!dummy_context) {
        Error("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    if (!wglMakeCurrent(hdc, dummy_context)) {
        Error("Failed to activate dummy OpenGL rendering context. : {}", GetLastError());
    }

    #elif defined(LINUX_PLT)

    Display* display = XOpenDisplay(nullptr);
    if (!display) return dummy_context;
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    static int32_t visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       ChannelBits,
        GLX_GREEN_SIZE,     ChannelBits,
        GLX_BLUE_SIZE,      ChannelBits,
        GLX_ALPHA_SIZE,     AlphaBits,
        GLX_DEPTH_SIZE,     DepthBufferBits,
        0
    };

    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, screen, visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        Error("Failed to get framebuffer config.");
    }

    XVisualInfo* vis = glXGetVisualFromFBConfig(display, fbc[0]);
    if (!vis) {
        XFree(fbc);
        Error("Failed to get visual info.");
    }

    Colormap cmap = XCreateColormap(display, root, vis->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = 0;
    
    Window win = XCreateWindow(display, root, 
        0, 0, 1, 1,
        0,
        vis->depth,
        InputOutput,
        vis->visual,
        CWColormap | CWBackPixmap | CWBorderPixel | CWEventMask,
        &swa
    );

    dummy_context = glXCreateContext(display, vis, NULL, True);

    XFree(fbc);
    XFree(vis);

    if (!dummy_context) {
        Error("Failed to create GLX context.");
    }

    if (!glXMakeCurrent(display, win, dummy_context)) {
        glXDestroyContext(display, dummy_context);
        XDestroyWindow(display, win);
        XCloseDisplay(display);
        Error("Can't glXMakeCurrent ctx");
    }

    #elif defined(WEB_PLT)
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;

    dummy_context = emscripten_webgl_create_context("#canvas", &attrs);
    
    if (dummy_context < 0 || 
        emscripten_webgl_make_context_current(dummy_context) != EMSCRIPTEN_RESULT_SUCCESS) {
            Error("Can't Make Opengl Ctx Current. ");
    }
    #endif

    #ifdef DEBUG
    #   define RESOLVEGL(name)\
        OpenGL::name = Function<decltype(&gl##name)>{};\
        OpenGL::name.m_Func  = reinterpret_cast<decltype(&gl##name)>(gl::GetProcAddress("gl"#name));\
        OpenGL::name.m_After = []([[maybe_unused]] std::string info) { GLenum err = glGetError(); if(err != GL_NO_ERROR) Info("[{}] {}", GL_ERR_to_string(err), info); };\
        OpenGL::name.m_Name  = "gl"#name
    #else
    #   define RESOLVEGL(name)\
        OpenGL::name = reinterpret_cast<decltype(&gl##name)>(gl::GetProcAddress("gl"#name))
    #endif

	GLFUNCS(RESOLVEGL)

    return dummy_context;
}

} //namespace gl