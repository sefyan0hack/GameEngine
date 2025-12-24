#pragma once
#include <vector>
#include <array>
#include <string>
#include <ctime>

#include "gl.hpp"
#include <engine_export.h>

class CWindow;

class ENGINE_EXPORT OpenGL
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

        std::string m_Vendor;
        std::string m_Renderer;
        std::vector<std::string> m_Extensions;
        inline static GLint m_MaxTextureUnits; // how many textures a `fragment` shader can sample
        inline static GLint m_MaxTextureSize;
        inline static GLint m_MaxTexture3DSize;
        inline static GLint m_MaxTextureCubeMapSize;
};