#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Utils.hpp>
#include <algorithm>

OpenGL::OpenGL([[maybe_unused]] const CWindow& window)
    : m_Window(window)
    , m_Context(create_opengl_context())
    , m_Major(0)
    , m_Minor(0)
{
    if (!make_current_opengl())
        throw Exception("Failed to make context current.");
    auto [w, h] = window.dims();
    
    gl::load_opengl_functions();
    
    set_viewport(0, 0, w, h);

    m_Major = gl::get_integer(GL_MAJOR_VERSION);
    m_Minor = gl::get_integer(GL_MINOR_VERSION);

    m_MaxTextureUnits       = gl::get_integer(GL_MAX_TEXTURE_IMAGE_UNITS);
    m_MaxTextureSize        = gl::get_integer(GL_MAX_TEXTURE_SIZE);
    m_MaxTexture3DSize      = gl::get_integer(GL_MAX_3D_TEXTURE_SIZE);
    m_MaxTextureCubeMapSize = gl::get_integer(GL_MAX_CUBE_MAP_TEXTURE_SIZE);

    auto vendor = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(gl::GetString(GL_RENDERER));

    m_Vendor = vendor ? vendor : "unknown";
    m_Renderer = renderer ? renderer : "unknown";

    #if defined(WINDOWS_PLT)
    auto exts = reinterpret_cast<const char*>(gl::GetExtensionsStringARB(window.surface()));
    #elif defined(LINUX_PLT)
    auto exts = reinterpret_cast<const char*>(glXQueryExtensionsString(window.display(), DefaultScreen(window.display())));
    #elif defined(WEB_PLT)
    auto exts = emscripten_webgl_get_supported_extensions();
    #elif defined(ANDROID_PLT)
    auto exts = eglQueryString(window.display(), EGL_EXTENSIONS);
    #endif

    m_Extensions = exts ? std::string_view{exts}
        | std::views::split(' ')
        | std::views::filter([](auto const &s) { return !s.empty(); })
        | std::ranges::to<std::vector<std::string>>() : decltype(m_Extensions){} ;

    gl::Enable(GL_DEPTH_TEST);
    gl::DepthFunc(GL_LESS);

    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl::Enable(GL_CULL_FACE);
    gl::CullFace(GL_BACK);

    #if defined(CORE_GL)
    gl::Enable(GL_LINE_SMOOTH);
    gl::Enable(GL_MULTISAMPLE);
    gl::Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    #endif

    debug::log(os::build_info());
    debug::log("=================================================================================");
    debug::log("Platform : {}, Arch : {}", os::system_name(), os::arch_name());
    debug::log("GL Version : Wanted:({}.{}) -> Got:({}.{})", gl::OPENGL_MAJOR_VERSION, gl::OPENGL_MINOR_VERSION, m_Major, m_Minor);
    debug::log("GL Vendor : {}", m_Vendor);
    debug::log("GL Renderer : {}", m_Renderer);
    debug::log("GL Exts : {}", m_Extensions);
    debug::log("Max Texture Units : {}", m_MaxTextureUnits);
    debug::log("Max Texture Size : {0} x {0}", m_MaxTextureSize);
    debug::log("Max Texture3D Size : {0} x {0} x {0}", m_MaxTexture3DSize);
    debug::log("Max TextureCubeMap Size : {0} x {0}", m_MaxTextureCubeMapSize);
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
