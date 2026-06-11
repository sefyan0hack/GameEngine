#pragma once
#include <vector>
#include <string>

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
        auto window() const -> const CWindow&;
        auto config() const -> GL_CFG ;
        auto context() const -> GL_CTX ;
        auto is_valid() const -> bool ;
        auto has_extension(const std::string& ext) const -> bool ;
        auto major_v() const -> GLint;
        auto minor_v() const -> GLint;
        auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void;

        auto vendor() -> std::string;
        auto renderer() -> std::string;
        auto extensions() -> std::vector<std::string>;
        static auto max_texture_units() -> GLint;
        static auto max_texture_size() -> GLint;
        static auto max_texture3d_size() -> GLint;
        static auto max_texturecubemap_size() -> GLint;
        static auto find_config([[maybe_unused]] const CWindow& window) -> GL_CFG;

    private:
        auto create_opengl_context() -> GL_CTX;
        auto make_current_opengl()  -> bool ;
        auto query_gl_extensions() const -> std::vector<std::string>;

    private:
        const CWindow& m_Window;
        GL_CFG m_Config;
        GL_CTX m_Context;
        GLint m_Major;
        GLint m_Minor;

        std::string m_Vendor;
        std::string m_Renderer;
        std::vector<std::string> m_Extensions;
};