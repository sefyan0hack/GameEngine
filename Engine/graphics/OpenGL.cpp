#include <algorithm>

#include "Window.hpp"
#include "OpenGL.hpp"
#include <core/Log.hpp>
#include <core/Utils.hpp>

namespace gl {

    auto get_proc_address(const char* name) -> void* {
        void *address = reinterpret_cast<void*>(XXXGetProcAddress(name));

        if(address == nullptr){
            address = utils::get_proc_address(OPENGL_MODULE_NAME, name);
        }

        if (address != nullptr) {
            debug::print("from LIB:`{}`: load function `{}` at : {}", OPENGL_MODULE_NAME, name, address);
        } else {
            throw Exception("Couldn't load {} function `{}`", OPENGL_MODULE_NAME, name);
        }

        return address;
    }

    auto get_integer(GLenum name) -> GLint
    {
        constexpr auto INVALID = std::numeric_limits<GLint>::max();

        GLint maxTexSize = INVALID;

        gl::GetIntegerv(name, &maxTexSize);

        if (maxTexSize != INVALID)
            return maxTexSize;
        else
            throw Exception("GetIntegerv Failed");
    }

    auto get_boolean(GLenum name) -> GLboolean
    {
        constexpr auto INVALID = std::numeric_limits<GLboolean>::max();

        GLboolean maxTexSize = INVALID;

        gl::GetBooleanv(name, &maxTexSize);

        if (maxTexSize != INVALID)
            return maxTexSize;
        else
            throw Exception("GetBooleanv Failed");
    }


    auto load_opengl_functions() -> void
    {
        #undef X
        #ifdef ROBUST_GL_CHECK
        #   define X(name)\
            name = Function<decltype(&gl##name)>{};\
            name.m_Func  = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name));\
            name.m_After = [](std::string info) { auto err = glGetError(); if(err != GL_NO_ERROR) if(!info.contains("glClear")) throw Exception("gl error id {} {}", err, info); };\
            name.m_Name  = "gl"#name
        #else
        #   define X(name)\
            name = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name))
        #endif

        GLFUNCS
    }
} //namespace gl

#if defined(WINDOWS_PLT)
#include "platform/windows/OpenGL.inl"
#elif defined(LINUX_PLT)
#include "platform/linux/OpenGL.inl"
#elif defined(WEB_PLT)
#include "platform/web/OpenGL.inl"
#endif

OpenGL::OpenGL([[maybe_unused]] const CWindow& window)
    : m_Context(create_opengl_context(window))
    , m_Major(0)
    , m_Minor(0)
    , m_CreationTime(std::time(nullptr))
{
    auto surface = window.surface();

    if (!make_current_opengl(window))
        throw Exception("Failed to make context current.");

    gl::load_opengl_functions();

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
    auto exts = reinterpret_cast<const char*>(wglGetExtensionsStringARB(surface));
    #elif defined(LINUX_PLT)
    auto exts = reinterpret_cast<const char*>(glXQueryExtensionsString(surface, DefaultScreen(surface)));
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
    debug::print("Platform : {}, Arch : {}", sys::host::name_str(), sys::host::arch_str());
    debug::print("GL Version : Wanted:({}.{}) -> Got:({}.{})", gl::GLMajorVersion, gl::GLMinorVersion, m_Major, m_Minor);
    debug::print("GL Vendor : {}", m_Vendor);
    debug::print("GL Renderer : {}", m_Renderer);
    debug::print("GL Exts : {}", utils::to_string(m_Extensions));
    debug::print("Max Texture Units : {}", m_MaxTextureUnits);
    debug::print("Max Texture Size : {0} x {0}", m_MaxTextureSize);
    debug::print("Max Texture3D Size : {0} x {0} x {0}", m_MaxTexture3DSize);
    debug::print("Max TextureCubeMap Size : {0} x {0}", m_MaxTextureCubeMapSize);
    debug::print("=================================================================================");

}

OpenGL::OpenGL(OpenGL &&other) noexcept
    : m_Context(std::exchange(other.m_Context, GL_CTX{}))
    , m_Major(std::exchange(other.m_Major, 0))
    , m_Minor(std::exchange(other.m_Minor, 0))
    , m_CreationTime(std::exchange(other.m_CreationTime, 0))
{
}

auto OpenGL::operator=(OpenGL &&other) noexcept -> OpenGL&
{
    if(this != &other){
        this->m_Context = std::exchange(other.m_Context, GL_CTX{});
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
