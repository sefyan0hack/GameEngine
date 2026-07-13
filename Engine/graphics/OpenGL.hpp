#pragma once
#include <string>
#include <utility>

#include "gl.inl"
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
        auto version() const -> std::pair<int32_t, int32_t>;
        auto is_current() const -> bool;
        auto make_current()  -> bool;
    
        static auto find_config([[maybe_unused]] const CWindow& window) -> GL_CFG;

        constexpr static enum class API { CORE, ES } api =
        #if defined(CORE_GL)
            API::CORE;
        #elif defined(ES_GL)
            API::ES;
        #endif

        constexpr static int32_t MIN_REQUIRED_MAJOR_VERSION = 3;
        constexpr static int32_t MIN_REQUIRED_MINOR_VERSION = api == API::CORE ? 3 : 0;

        constexpr static bool   DEBUG = true;
        constexpr static size_t MSAA  = 2;

    private:
        auto create_context() -> GL_CTX;
        auto enable_debug() const -> void;
        auto resolve_function(const char* name) -> void*;
        auto load_functions() -> void;

    private:
        const CWindow& m_Window;
        GL_CFG m_Config;
        GL_CTX m_Context;
        int32_t m_Major;
        int32_t m_Minor;
};