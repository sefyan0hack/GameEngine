#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>
#include <algorithm>
#include <ranges>

OpenGL::OpenGL([[maybe_unused]] const CWindow& window)
    : m_Window(window)
    , m_Config(find_config(m_Window))
    , m_Context(create_opengl_context())
    , m_Major(0)
    , m_Minor(0)
{
    if (!make_current_opengl())
        throw Exception("Failed to make context current.");
    auto [w, h] = window.dims();

    gl::load_opengl_functions();

    set_viewport(0, 0, w, h);

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);

    if (m_Major < gl::OPENGL_MIN_REQUIRED_MAJOR_VERSION ||
    (m_Major == gl::OPENGL_MIN_REQUIRED_MAJOR_VERSION &&
     m_Minor < gl::OPENGL_MIN_REQUIRED_MINOR_VERSION))
    {
        throw Exception(
            "Min required OpenGL version is {}.{} but got {}.{}",
            gl::OPENGL_MIN_REQUIRED_MAJOR_VERSION,
            gl::OPENGL_MIN_REQUIRED_MINOR_VERSION,
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

    debug::log(os::build_info());
    debug::log("=================================================================================");
    debug::log("Platform Name: {}", os::name());
    debug::log("Platform Arch: {} ({}) bit", os::arch(), os::bits());
    debug::log("GL Version : {}.{}", m_Major, m_Minor);
    debug::log("GL Vendor : {}", m_Vendor);
    debug::log("GL Renderer : {}", m_Renderer);
    debug::log("GL Exts : {} Extention", m_Extensions.size());
    debug::log("GL Texture Units : {}", max_texture_units());
    debug::log("GL Texture Size : {0} x {0}", max_texture_size());
    debug::log("GL Texture3D Size : {0} x {0} x {0}", max_texture3d_size());
    debug::log("GL TextureCubeMap Size : {0} x {0}", max_texturecubemap_size());
    debug::log("=================================================================================");
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

auto OpenGL::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void
{
    gl::Viewport(x, y, width, height);
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
    GLint r = 0;
    gl::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &r);
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

auto OpenGL::query_gl_extensions() const -> std::vector<std::string> 
{
    int numExtensions = 0;
    gl::GetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    std::vector<std::string> exts;

    for (int i = 0; i < numExtensions; ++i) {
        exts.push_back((const char*)gl::GetStringi(GL_EXTENSIONS, i));
    }

    return exts;
}