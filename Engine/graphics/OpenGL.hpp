#pragma once
#include <vector>
#include <string>
#include <ctime>

#include "gl.h"
#include "GApi.hpp"
#include <core/Function.hpp>
#include <engine_export.h>

class CWindow;

namespace gl {
    constexpr auto OPENGL_MODULE_NAME = OPENGL_LIB;

    inline static auto OPENGL_FUNCTIONS_NAME = [](){
        static std::vector<const char*> r;
        #undef X 
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

class ENGINE_API OpenGL final: public GApi
{
    public:
        explicit OpenGL(const CWindow& window);
        OpenGL(const OpenGL& other) = delete;
        OpenGL(OpenGL&& other) noexcept;
        ~OpenGL();

        auto operator = (const OpenGL& other) -> OpenGL& = delete;
        auto operator = (OpenGL&& other)  noexcept -> OpenGL&;

        auto operator == (const OpenGL& other) const -> bool;
        auto operator != (const OpenGL& other) const -> bool;
        operator bool () const;

    public:
        auto context() const -> GCTX override ;
        auto is_valid() const -> bool override ;
        auto creation_time() const -> std::time_t override ;
        auto has_extension(const std::string& ext) const -> bool override ;
        auto major_v() const -> GLint;
        auto minor_v() const -> GLint;
        
        static auto load_opengl_functions() -> void;
        static auto vendor() -> std::string;
        static auto renderer() -> std::string;
        static auto extensions() -> std::vector<std::string>;
        static auto max_texture_units() -> GLint;
        static auto max_texture_size() -> GLint;
        static auto max_texture3d_size() -> GLint;
        static auto max_texturecubemap_size() -> GLint;
        
    private:
        auto create_opengl_context([[maybe_unused]] const CWindow& window) -> GL_CTX;
        auto make_current_opengl([[maybe_unused]] const CWindow& window)  -> bool ;

    private:
        GL_CTX m_Context;
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
};

auto get_integer(GLenum name) -> GLint;
auto get_boolean(GLenum name) -> GLboolean;

} //namespace g