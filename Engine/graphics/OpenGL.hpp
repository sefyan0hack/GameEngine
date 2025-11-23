#pragma once
#include <vector>
#include <array>
#include <string>
#include <ctime>

#include "gl.h"
#include <core/Function.hpp>
#include <engine_export.h>

class CWindow;

namespace gl {
    #undef X 
    #define X(name) "gl"#name, 

    constexpr auto OPENGL_MODULE_NAME = OPENGL_LIB;
    constexpr auto OPENGL_FUNCTIONS_NAME = { GLFUNCS };
    constexpr auto OPENGL_FUNCTIONS_COUNT = OPENGL_FUNCTIONS_NAME.size();
    constexpr int32_t OPENGL_MAJOR_VERSION = 3;
    constexpr int32_t OPENGL_MINOR_VERSION = 3;

    constexpr int32_t DepthBufferBits   = 32;
    constexpr int32_t StencilBufferBits = 8;
    constexpr int32_t ChannelBits       = 8;
    constexpr int32_t AlphaBits         = 8;
    
    auto ENGINE_API get_proc_address(const char* name) -> void*;
    auto ENGINE_API get_integer(GLenum name) -> GLint;
    auto ENGINE_API get_boolean(GLenum name) -> GLboolean;
    auto ENGINE_API load_opengl_functions() -> void;

} //namespace gl

class ENGINE_API OpenGL
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
        auto context() const -> GL_CTX ;
        auto is_valid() const -> bool ;
        auto creation_time() const -> std::time_t ;
        auto has_extension(const std::string& ext) const -> bool ;
        auto major_v() const -> GLint;
        auto minor_v() const -> GLint;
        
        auto vendor() -> std::string;
        auto renderer() -> std::string;
        auto extensions() -> std::vector<std::string>;
        auto max_texture_units() -> GLint;
        auto max_texture_size() -> GLint;
        auto max_texture3d_size() -> GLint;
        auto max_texturecubemap_size() -> GLint;
        
    private:
        auto create_opengl_context([[maybe_unused]] const CWindow& window) -> GL_CTX;
        auto make_current_opengl([[maybe_unused]] const CWindow& window)  -> bool ;

    private:
        GL_CTX m_Context;
        GLint m_Major;
        GLint m_Minor;
        std::time_t m_CreationTime;

        std::string m_Vendor;
        std::string m_Renderer;
        std::vector<std::string> m_Extensions;
        GLint m_MaxTextureUnits; // how many textures a `fragment` shader can sample
        GLint m_MaxTextureSize;
        GLint m_MaxTexture3DSize;
        GLint m_MaxTextureCubeMapSize;
};