#pragma once
#include <vector>
#include <string>
#include <ctime>

#include "gl.h"
#include <core/Function.hpp>

namespace gl {
    constexpr auto OPENGL_MODULE_NAME = OPENGL_LIB;

    inline static auto OPENGL_FUNCTIONS_NAME = [](){
        static std::vector<const char*> r;

        #define X(name) r.push_back("gl"#name)
        GLFUNCS

        return r;
    }();

    inline static auto OPENGL_FUNCTIONS_COUNT = OPENGL_FUNCTIONS_NAME.size();

    auto get_proc_address(const char* name) -> void*;

    constexpr int32_t DepthBufferBits   = 32;
    constexpr int32_t StencilBufferBits = 8;
    constexpr int32_t ChannelBits       = 8;
    constexpr int32_t AlphaBits         = 8;

    constexpr int32_t  GLMajorVersion   = 3;
    constexpr int32_t  GLMinorVersion   = 3;

class OpenGL
{
    public:
        explicit OpenGL(H_WIN window, H_SRF surface);
        OpenGL(const OpenGL& other);
        OpenGL(OpenGL&& other) noexcept;
        ~OpenGL();
        auto operator = (const OpenGL& other) -> OpenGL&;
        auto operator = (OpenGL&& other)  noexcept -> OpenGL&;
        auto operator == (const OpenGL& other) const -> bool;
        auto operator != (const OpenGL& other) const -> bool;
        operator bool () const;

    public:
        auto context() const -> GLCTX;
        auto surface() const -> H_SRF;
        auto major_v() const -> GLint;
        auto minor_v() const -> GLint;
        auto is_valid() const -> bool;
        auto creation_time() const -> std::time_t;
        auto has_extension(const std::string& ext) const -> bool;

        static auto vendor() -> std::string;
        static auto renderer() -> std::string;
        static auto extensions() -> std::vector<std::string>;
        static auto max_texture_units() -> GLint;
        static auto max_texture_size() -> GLint;
        static auto max_texture3d_size() -> GLint;
        static auto max_texturecubemap_size() -> GLint;
        
        static auto dummy_ctx() -> GLCTX;
    private:
        auto init_opengl()  -> void ;
        auto make_current_opengl(GLCTX ctx, [[maybe_unused]] H_SRF srf, [[maybe_unused]] H_WIN win)  -> bool ;

    private:
        GLCTX m_Context;
        H_SRF m_Surface;
        GLint m_Major;
        GLint m_Minor;
        std::time_t m_CreationTime;

        inline static std::string m_Vendor{};
        inline static std::string m_Renderer{};
        inline static std::vector<std::string> m_Extensions{};
        inline static GLint m_MaxTextureUnits{}; // how many textures a `fragment` shader can sample
        inline static GLint m_MaxTextureSize{};
        inline static GLint m_MaxTexture3DSize{};
        inline static GLint m_MaxTextureCubeMapSize{};

    public:
        #undef X
        #ifdef ROBUST_GL_CHECK
        #   define X(name)\
            inline static Function<decltype(&gl##name)> name
        #else
        #   define X(name)\
            inline static decltype(&gl##name) name = Function<decltype(&gl##name)>::default_
        #endif

        GLFUNCS

};

auto get_integer(GLenum name) -> GLint;
auto get_boolean(GLenum name) -> GLboolean;


#undef X
#ifdef ROBUST_GL_CHECK
#   define X(name)\
inline Function<decltype(&gl##name)>& name = OpenGL::name
#else
#   define X(name)\
inline decltype(&gl##name)& name = OpenGL::name
#endif

GLFUNCS

} //namespace g