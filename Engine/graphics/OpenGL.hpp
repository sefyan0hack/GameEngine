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
        auto operator = (const OpenGL& other) -> OpenGL& = delete;
        ~OpenGL();

    public:
        auto context() const -> GL_CTX ;
        auto window() const -> const CWindow&;
        auto is_valid() const -> bool ;
        auto creation_time() const -> std::time_t ;
        auto has_extension(const std::string& ext) const -> bool ;
        auto major_v() const -> GLint;
        auto minor_v() const -> GLint;
        auto set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void;
        
        auto vendor() -> std::string;
        auto renderer() -> std::string;
        auto extensions() -> std::vector<std::string>;
        static auto max_texture_units() -> GLint;
        static auto max_texture_size() -> GLint;
        static auto max_texture3d_size() -> GLint;
        static auto max_texturecubemap_size() -> GLint;
        
    private:
        auto create_opengl_context() -> GL_CTX;
        auto make_current_opengl()  -> bool ;

    private:
        const CWindow& m_Window;
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