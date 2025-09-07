#include <algorithm>

#include "OpenGL.hpp"
#include "Log.hpp"
#include "Utils.hpp"

namespace gl {

#ifdef ROBUST_GL_CHECK
inline static auto After_Func = []([[maybe_unused]] std::string info) {
    GLenum err = glGetError();
    if(err != GL_NO_ERROR){
        if(!info.contains("glClear"))
            throw CException("[{}] {}", gl_err_to_string(err), info);
    }
};
#endif

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
    auto pixel_format = ChoosePixelFormat(m_Surface, &pfd);
    if (!pixel_format) {
        throw CException("Failed to find a suitable pixel format. : {}", GetLastError());
    }
    if (!SetPixelFormat(m_Surface, pixel_format, &pfd)) {
        throw CException("Failed to set the pixel format. : {}", GetLastError());
    }

    GLCTX dummy_context = nullptr;

    dummy_context = wglCreateContext(m_Surface);
    if (!dummy_context) {
        throw CException("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    if (!wglMakeCurrent(m_Surface, dummy_context)) {
        throw CException("Failed to activate dummy OpenGL rendering context. : {}", GetLastError());
    }

    wglGetExtensionsStringARB  = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));
    wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

    if (!wglGetExtensionsStringARB){
        throw CException("Failed to load wglGetExtensionsStringARB. : {}", GetLastError());
    }

    if(!wglCreateContextAttribsARB) {
        throw CException("Failed to load wglCreateContextAttribsARB. : {}", GetLastError());
    }

    int32_t gl_attribs[] = { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, GLMajorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, GLMinorVersion,
    #ifdef DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    #endif
        0,
    };


    GLCTX opengl_context = nullptr;
    if (nullptr == (opengl_context = wglCreateContextAttribsARB(m_Surface, nullptr, gl_attribs))) {
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            throw CException("Unsupported GL Version {}.{}", GLMajorVersion, GLMinorVersion);
        }
        throw CException("Failed to create the final rendering context!");
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
    GLXFBConfig* fbc = glXChooseFBConfig(m_Surface, DefaultScreen(m_Surface), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        throw CException("Failed to get framebuffer config.");
    }

    XVisualInfo* visInfo = glXGetVisualFromFBConfig(m_Surface, fbc[0]);
    if (!visInfo) {
        XFree(fbc);
        throw CException("Failed to get visual info.");
    }

    int32_t contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, GLMajorVersion,
        GLX_CONTEXT_MINOR_VERSION_ARB, GLMinorVersion,
        #ifdef DEBUG
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB | GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        #endif
        0
    };

    glXCreateContextAttribsARB = (decltype(glXCreateContextAttribsARB))glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    m_Context = glXCreateContextAttribsARB(m_Surface, fbc[0], nullptr, True, contextAttribs);
    XFree(fbc);
    XFree(visInfo);

    if (!m_Context) {
        throw CException("Failed to create GLX context.");
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

    m_Context = emscripten_webgl_create_context(m_Surface, &attrs);

    if (m_Context <= 0) {
        throw CException("Failed to create WebGL context: error {}", static_cast<int32_t>(m_Context));
    }
}

#endif

OpenGL::OpenGL([[maybe_unused]] H_WIN window, H_SRF surface)
    : m_Context(GLCTX{})
    , m_Surface(surface)
    , m_Major(0)
    , m_Minor(0)
    , m_CreationTime(std::time(nullptr))
{
    #if defined(WINDOWS_PLT)
    init_opengl_win32();
    if (!wglMakeCurrent(m_Surface, m_Context)){
		throw CException("Failed to make context current.");
	}
    #elif defined(LINUX_PLT)
    init_opengl_linux();
    if (!glXMakeCurrent(m_Surface, window, m_Context)) {
        throw CException("Failed to make context current.");
    }
    #elif defined(WEB_PLT)
    init_opengl_web();
    if (emscripten_webgl_make_context_current(m_Context) != EMSCRIPTEN_RESULT_SUCCESS) {
        throw CException("Failed to make WebGL context current.");
    }
    #endif

    #ifdef ROBUST_GL_CHECK
    #   define RESOLVEGL(name)\
        OpenGL::name = Function<decltype(&gl##name)>{};\
        OpenGL::name.m_Func  = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name));\
        OpenGL::name.m_After = After_Func;\
        OpenGL::name.m_Name  = "gl"#name
    #else
    #   define RESOLVEGL(name)\
        OpenGL::name = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name))
    #endif

	GLFUNCS(RESOLVEGL)

    m_Major = get_integer(GL_MAJOR_VERSION);
    m_Minor = get_integer(GL_MINOR_VERSION);

    m_MaxTextureUnits       = get_integer(GL_MAX_TEXTURE_IMAGE_UNITS);
    m_MaxTextureSize        = get_integer(GL_MAX_TEXTURE_SIZE);
    m_MaxTexture3DSize      = get_integer(GL_MAX_3D_TEXTURE_SIZE);
    m_MaxTextureCubeMapSize = get_integer(GL_MAX_CUBE_MAP_TEXTURE_SIZE);

    auto vendor = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(gl::GetString(GL_RENDERER));
    m_Vendor = vendor ? vendor : "unknown";
    m_Renderer = renderer ? renderer : "unknown";

    #if defined(WINDOWS_PLT)
    auto exts = reinterpret_cast<const char*>(wglGetExtensionsStringARB(m_Surface));
    #elif defined(LINUX_PLT)
    auto exts = reinterpret_cast<const char*>(glXQueryExtensionsString(m_Surface, DefaultScreen(m_Surface)));
    #elif defined(WEB_PLT)
    auto exts = emscripten_webgl_get_supported_extensions();
    #endif

    m_Extensions = exts ? utils::split(exts, " ") : decltype(m_Extensions){} ;
    
    gl::Enable(GL_DEPTH_TEST);
    gl::DepthFunc(GL_LESS);

    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl::Enable(GL_CULL_FACE);
    gl::CullFace(GL_BACK);

    #if !defined(WEB_PLT)
    gl::Enable(GL_LINE_SMOOTH);
    gl::Enable(GL_MULTISAMPLE);
    gl::Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    #endif

    debug::print("=================================================================================");
    debug::print("Platform : {}, Arch : {}", sys::TargetName, sys::ArchName);
    debug::print("GL Version : Wanted:({}.{}) -> Got:({}.{})", GLMajorVersion, GLMinorVersion, m_Major, m_Minor);
    debug::print("GL Vendor : {}", m_Vendor);
    debug::print("GL Renderer : {}", m_Renderer);
    debug::print("GL Exts : {}", utils::to_string(m_Extensions));
    debug::print("Max Texture Units : {}", m_MaxTextureUnits);
    debug::print("Max Texture Size : {0} x {0}", m_MaxTextureSize);
    debug::print("Max Texture3D Size : {0} x {0} x {0}", m_MaxTexture3DSize);
    debug::print("Max TextureCubeMap Size : {0} x {0}", m_MaxTextureCubeMapSize);
    debug::print("=================================================================================");
}

OpenGL::OpenGL(const OpenGL &other)
    : m_Context(GLCTX{})
    , m_Surface(other.m_Surface)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
{
    #if defined(WINDOWS_PLT)
    auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    if(tst != TRUE) throw CException("couldn't Copy Opengl Context");
    #elif defined(LINUX_PLT)
    glXCopyContext(this->m_Surface, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
    // no error check for now  ` X11 ` Shit
    #endif
}

auto OpenGL::operator=(const OpenGL &other) -> OpenGL&
{
    if(this != &other){
        this->m_Context = GLCTX{};
        this->m_Surface = other.m_Surface;    
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = std::time(nullptr);

        #if defined(WINDOWS_PLT)
        auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        if(tst != TRUE) throw CException("couldn't Copy Opengl Context");
        #elif defined(LINUX_PLT)
        glXCopyContext(this->m_Surface, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
        // no error check for now  ` X11 ` Shit
        #endif //_WIN32
    }
    return *this;
}

OpenGL::OpenGL(OpenGL &&other) noexcept
    : m_Context(std::exchange(other.m_Context, GLCTX{}))
    , m_Surface(std::exchange(other.m_Surface, nullptr))
    , m_Major(std::exchange(other.m_Major, 0))
    , m_Minor(std::exchange(other.m_Minor, 0))
    , m_CreationTime(std::exchange(other.m_CreationTime, 0))
{
}

auto OpenGL::operator=(OpenGL &&other) noexcept -> OpenGL&
{
    if(this != &other){
        this->m_Context = std::exchange(other.m_Context, GLCTX{});
        this->m_Surface = std::exchange(other.m_Surface, nullptr);
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
    return is_valid();
}

OpenGL::~OpenGL()
{
    #if defined(WINDOWS_PLT)
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_Context);
    #elif defined(LINUX_PLT)
    glXMakeCurrent(m_Surface, Window{},  GLCTX{});
    glXDestroyContext(m_Surface, m_Context);
    #elif defined(WEB_PLT)
    if (m_Context > 0) {
        emscripten_webgl_destroy_context(m_Context);
    }
    #endif
}

auto OpenGL::context() const -> GLCTX
{
    return m_Context;
}

auto OpenGL::surface() const -> H_SRF
{
    return m_Surface;
}

auto OpenGL::major_v() const -> GLint
{
    return m_Major;
}
auto OpenGL::minor_v() const -> GLint
{
    return m_Minor;
}

auto OpenGL::is_valid() const -> bool
{
    return m_Context != GLCTX{};
}

auto OpenGL::creation_time() const -> std::time_t
{
    return m_CreationTime;
}

auto OpenGL::has_extension(const std::string &ext) const -> bool
{
    return std::ranges::contains(m_Extensions, ext);
}

auto OpenGL::vendor() -> std::string
{
    return m_Vendor;
}

auto OpenGL::renderer() -> std::string
{
    return m_Renderer;
}


auto OpenGL::extensions() -> std::vector<std::string>
{
    return m_Extensions;
}

auto OpenGL::max_texture_units() -> GLint
{
    return m_MaxTextureUnits;
}

auto OpenGL::max_texture_size() -> GLint
{
    return m_MaxTextureSize;
}

auto OpenGL::max_texture3d_size() -> GLint
{
    return m_MaxTexture3DSize;
}

auto OpenGL::max_texturecubemap_size() -> GLint
{
    return m_MaxTextureCubeMapSize;
}



auto get_proc_address(const char* name) -> void* {
    void *address = reinterpret_cast<void*>(XXXGetProcAddress(name));

    if(address == nullptr){
        address = utils::get_proc_address(OPENGL_MODULE_NAME, name);
    }

    if (address != nullptr) {
        debug::print("from LIB:`{}`: load function `{}` at : {}", OPENGL_MODULE_NAME, name, address);
    } else {
        throw CException("Couldn't load {} function `{}`", OPENGL_MODULE_NAME, name);
    }

    return address;
}


auto gl::OpenGL::dummy_ctx() -> GLCTX
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
                throw CException("Can't register Window class: {}", error);
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
        throw CException("Can't Create Window : {}", GetLastError());
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
        throw CException("Failed to find a suitable pixel format. : {}", GetLastError());
    }
    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        throw CException("Failed to set the pixel format. : {}", GetLastError());
    }

    dummy_context = wglCreateContext(hdc);
    if (!dummy_context) {
        throw CException("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    if (!wglMakeCurrent(hdc, dummy_context)) {
        throw CException("Failed to activate dummy OpenGL rendering context. : {}", GetLastError());
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
        throw CException("Failed to get framebuffer config.");
    }

    XVisualInfo* vis = glXGetVisualFromFBConfig(display, fbc[0]);
    if (!vis) {
        XFree(fbc);
        throw CException("Failed to get visual info.");
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
        throw CException("Failed to create GLX context.");
    }

    if (!glXMakeCurrent(display, win, dummy_context)) {
        glXDestroyContext(display, dummy_context);
        XDestroyWindow(display, win);
        XCloseDisplay(display);
        throw CException("Can't glXMakeCurrent ctx");
    }

    #elif defined(WEB_PLT)
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;

    dummy_context = emscripten_webgl_create_context("#canvas", &attrs);
    
    if (dummy_context < 0 || 
        emscripten_webgl_make_context_current(dummy_context) != EMSCRIPTEN_RESULT_SUCCESS) {
            throw CException("Can't Make Opengl Ctx Current. ");
    }
    #endif

    #ifdef ROBUST_GL_CHECK
    #   define RESOLVEGL(name)\
        OpenGL::name = Function<decltype(&gl##name)>{};\
        OpenGL::name.m_Func  = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name));\
        OpenGL::name.m_After = After_Func;\
        OpenGL::name.m_Name  = "gl"#name
    #else
    #   define RESOLVEGL(name)\
        OpenGL::name = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name))
    #endif

	GLFUNCS(RESOLVEGL)

    return dummy_context;
}



auto get_integer(GLenum name) -> GLint
{
    constexpr auto INVALID = std::numeric_limits<GLint>::max();

    GLint maxTexSize = INVALID;

    gl::GetIntegerv(name, &maxTexSize);

    if (maxTexSize != INVALID)
        return maxTexSize;
    else
        throw CException("GetIntegerv Failed");
}

auto get_boolean(GLenum name) -> GLboolean
{
    constexpr auto INVALID = std::numeric_limits<GLboolean>::max();

    GLboolean maxTexSize = INVALID;

    gl::GetBooleanv(name, &maxTexSize);

    if (maxTexSize != INVALID)
        return maxTexSize;
    else
        throw CException("GetBooleanv Failed");
}

} //namespace gl