#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>

#define PACK(x, y) ((uint32_t(x) << 16) | (uint32_t(y) & 0xFFFF))

auto get_info(GLenum name) -> GLint
{
    GLint r = 0;
    gl::GetIntegerv(name, &r);
    return r;
}

#define gl_info(name) logg::info(#name" : {}", get_info(name))

OpenGL::OpenGL([[maybe_unused]] const CWindow& window)
    : m_Window(window)
    , m_Config(find_config(m_Window))
    , m_Context(create_context())
    , m_Major(0)
    , m_Minor(0)
{
    if (make_current()) load_functions();
    else throw Exception("Failed to make context current.");

    auto [w, h] = window.dims();

    gl::Viewport(0, 0, w, h);

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);

    if (PACK(m_Major, m_Minor) < PACK(MIN_REQUIRED_MAJOR_VERSION, MIN_REQUIRED_MINOR_VERSION))
    {
        throw Exception(
            "Min required OpenGL version is {}.{} but got {}.{}",
            MIN_REQUIRED_MAJOR_VERSION,
            MIN_REQUIRED_MINOR_VERSION,
            m_Major,
            m_Minor
        );
    }

    auto vendor = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(gl::GetString(GL_RENDERER));
    auto version = reinterpret_cast<const char*>(gl::GetString(GL_VERSION));
    auto glsl_version = reinterpret_cast<const char*>(gl::GetString(GL_SHADING_LANGUAGE_VERSION));

    auto Vendor = vendor ? vendor : "unknown";
    auto Renderer = renderer ? renderer : "unknown";
    auto Version = version ? version : "unknown";
    auto GlslVersion = glsl_version ? glsl_version : "unknown";

    gl::Enable(GL_DEPTH_TEST);
    gl::DepthFunc(GL_LESS);

    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl::Enable(GL_CULL_FACE);
    gl::CullFace(GL_BACK);

    #if defined(CORE_GL)
    gl::Enable(GL_MULTISAMPLE);
    gl::Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    #endif

    if constexpr (DEBUG) enable_debug();

    logg::info(os::build_info());
    logg::info("===================================[GL Info]=========================================");
    logg::info("Platform Name: {}", os::name());
    logg::info("Platform Arch: {} ({}) bit", os::arch(), os::bits());
    logg::info("GL Version : {}", Version);
    logg::info("GL Shading Version : {}", GlslVersion);
    logg::info("GL Vendor : {}", Vendor);
    logg::info("GL Renderer : {}", Renderer);
    logg::info("GL Debug : {}", DEBUG ? "true" : "false");
    logg::info("===================================[GL Extention]=========================================");
    logg::info(extensions());
    logg::info("===================================[Metrics]==========================================");
    gl_info(GL_MAX_TEXTURE_SIZE);
    gl_info(GL_MAX_3D_TEXTURE_SIZE);
    gl_info(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    gl_info(GL_MAX_TEXTURE_IMAGE_UNITS);
    gl_info(GL_MAX_RENDERBUFFER_SIZE);
    gl_info(GL_SAMPLES);
    gl_info(GL_MAX_SAMPLES);
    gl_info(GL_MAX_COLOR_ATTACHMENTS);
}

auto OpenGL::window() const -> const CWindow&
{
    return m_Window;
}

auto OpenGL::context() const -> GL_CTX
{
    return m_Context;
}

auto OpenGL::config() const -> GL_CFG
{
    return m_Config;
}

auto OpenGL::version() const -> std::pair<int32_t, int32_t>
{
 return {m_Major, m_Minor};
}

auto OpenGL::is_current() const -> bool
{
    return gl::GetCurrentContext() == m_Context;
}

auto OpenGL::extension_supported(const std::string &ext) const -> bool
{
    return extensions().contains(ext);
}

auto OpenGL::extensions() const-> std::string
{
    static std::string extensions = [this]()
    {
        int32_t count{};
        gl::GetIntegerv(GL_NUM_EXTENSIONS, &count);
        std::string exts;

        for (int32_t i = 0; i < count; ++i) {
            exts += std::format("{} ", (const char*)gl::GetStringi(GL_EXTENSIONS, i));
        }
        
        #if defined(CORE_GL)
        GET_GLEXT_FUNCTION_NO_THROW(wglGetExtensionsStringARB);
        if(wglGetExtensionsStringARB_ext)
        exts += wglGetExtensionsStringARB_ext(m_Window.surface());
        #endif

        return exts;
    }();
    return extensions;
}


auto OpenGL::enable_debug() const -> void
{
    // Enable Opengl debug
    #if defined(CORE_GL)

    auto messgae_callback_func = +[](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *) -> void
    {
        logg::error("{} {} {} {} {}", source, type, id, severity, message);
    };

    if (PACK(m_Major, m_Minor) >= PACK(4,3) || extension_supported("GL_KHR_debug")) {

        GET_GLEXT_FUNCTION_THROW(glDebugMessageCallback);
        gl::Enable(GL_DEBUG_OUTPUT);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback_ext(messgae_callback_func, nullptr);

    } else if(extension_supported("GL_ARB_debug_output")) {
        GET_GLEXT_FUNCTION_THROW(glDebugMessageCallbackARB);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

        glDebugMessageCallbackARB_ext(messgae_callback_func, nullptr);
    }
    #endif
}

auto OpenGL::resolve_function(const char* name) -> void* {
    void *address = gl::GetProcAddress<void*>(name);

    if(address == nullptr){
        address = os::get_proc_address(EG_OPENGL_MODULE_NAME, name);
    }

    if (address == nullptr) {
        throw Exception("Couldn't load {} function `{}`", EG_OPENGL_MODULE_NAME, name);
    }

    return address;
}

auto OpenGL::load_functions() -> void
{
    #define FUNC_GL_X(name) gl::name = reinterpret_cast<decltype(&gl##name)>(resolve_function("gl"#name));
    FUNCTIONS_GL_LIST
    #undef FUNC_GL_X
}