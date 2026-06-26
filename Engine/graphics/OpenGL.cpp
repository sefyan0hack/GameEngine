#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>

#define PACK(x, y) ((uint32_t(x) << 16) | (uint32_t(y) & 0xFFFF))

OpenGL::OpenGL([[maybe_unused]] const CWindow& window)
    : m_Window(window)
    , m_Config(find_config(m_Window))
    , m_Context(create_opengl_context())
    , m_Major(0)
    , m_Minor(0)
{
    if (!make_current_opengl()) throw Exception("Failed to make context current.");

    gl::load_opengl_functions();
    auto [w, h] = window.dims();

    set_viewport(0, 0, w, h);

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);

    if (m_Major < gl::MIN_REQUIRED_MAJOR_VERSION ||
    (m_Major == gl::MIN_REQUIRED_MAJOR_VERSION &&
     m_Minor < gl::MIN_REQUIRED_MINOR_VERSION))
    {
        throw Exception(
            "Min required OpenGL version is {}.{} but got {}.{}",
            gl::MIN_REQUIRED_MAJOR_VERSION,
            gl::MIN_REQUIRED_MINOR_VERSION,
            m_Major,
            m_Minor
        );
    }
    auto vendor = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(gl::GetString(GL_RENDERER));

    m_Vendor = vendor ? vendor : "unknown";
    m_Renderer = renderer ? renderer : "unknown";
    m_Extensions = query_gl_extensions();

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

    if constexpr (DEBUG) regester_debug_func();

    logg::info(os::build_info());
    logg::info("===================================[GL Info]=========================================");
    logg::info("Platform Name: {}", os::name());
    logg::info("Platform Arch: {} ({}) bit", os::arch(), os::bits());
    logg::info("GL Version : {}.{}", m_Major, m_Minor);
    logg::info("GL Vendor : {}", m_Vendor);
    logg::info("GL Renderer : {}", m_Renderer);
    logg::info("GL Debug : {}", DEBUG ? "true" : "false");
    logg::info("===================================[GL Extention]=========================================");
    logg::info(m_Extensions);
    logg::info("===================================[Metrics]==========================================");
    logg::info("MSAA Level: {}", MSAA);
    logg::info("Max Texture Units : {}", max_texture_units());
    logg::info("Max Texture Size : {0} x {0}", max_texture_size());
    logg::info("Max Texture3D Size : {0} x {0} x {0}", max_texture3d_size());
    logg::info("Max TextureCubeMap Size : {0} x {0}", max_texturecubemap_size());
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
    return m_Context != GL_CTX{};
}

auto OpenGL::set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void
{
    gl::Viewport(x, y, width, height);
}

auto OpenGL::extension_supported(const std::string &ext) const -> bool
{
    return m_Extensions.contains(ext);
}

auto OpenGL::vendor() const-> std::string
{
    return m_Vendor;
}

auto OpenGL::renderer() const-> std::string
{
    return m_Renderer;
}

auto OpenGL::extensions() const-> std::string
{
    return m_Extensions;
}

auto OpenGL::max_texture_units() -> GLint
{
    GLint r = 0;
    gl::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &r); // fragment shader limit
    return r;
}

auto OpenGL::max_texture_size() -> GLint
{
    GLint r = 0;
    gl::GetIntegerv(GL_MAX_TEXTURE_SIZE, &r);
    return r;
}

auto OpenGL::max_texture3d_size() -> GLint
{
    GLint r = 0;
    gl::GetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &r);
    return r;
}

auto OpenGL::max_texturecubemap_size() -> GLint
{
    GLint r = 0;
    gl::GetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &r);
    return r;
}

auto OpenGL::query_gl_extensions() const -> std::string
{
    int32_t count{};
    gl::GetIntegerv(GL_NUM_EXTENSIONS, &count);

    std::string extensions;

    for (int32_t i = 0; i < count; ++i) {
        extensions += std::format("{} ", (const char*)gl::GetStringi(GL_EXTENSIONS, i));
    }

    #if defined(CORE_GL)
    GET_GLEXT_FUNCTION_NO_THROW(wglGetExtensionsStringARB);
    if(wglGetExtensionsStringARB_ext)
        extensions += wglGetExtensionsStringARB_ext(m_Window.surface());
    #endif
    return extensions;
}

auto OpenGL::regester_debug_func() const -> void
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