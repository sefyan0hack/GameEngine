#pragma once

#include <core/gl.h>

namespace gl {
    constexpr auto OPENGL_MODULE_NAME = OPENGL_LIB;

    inline static auto OPENGL_FUNCTIONS_NAME = [](){
        static std::vector<const char*> r;
        static std::size_t i = 0;

        #define APPEND_OPENGL_FUNCTIONS(name) r.push_back("gl"#name)
        GLFUNCS(APPEND_OPENGL_FUNCTIONS)

        return r;
    }();

    inline static auto OPENGL_FUNCTIONS_COUNT = OPENGL_FUNCTIONS_NAME.size();

    auto GetProcAddress(const char* name) -> void*;

    constexpr int32_t DepthBufferBits   = 24;
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
        auto Context() const -> GLCTX;
        auto Surface() const -> H_SRF;
        auto MajorV() const -> GLint;
        auto MinorV() const -> GLint;
        auto isValid() const -> bool;
        auto CreationTime() const -> std::time_t;
        auto HasExtension(const std::string& ext) const -> bool;

        auto GetInteger(GLenum name) -> std::optional<GLint>;

        static auto Vendor() -> std::string;
        static auto Renderer() -> std::string;
        static auto Extensions() -> std::vector<std::string>;
        static auto MaxTextureUnits() -> GLint;
        static auto MaxTextureSize() -> GLint;
        static auto MaxTexture3DSize() -> GLint;
        static auto MaxTextureCubeMapSize() -> GLint;
        
        static auto DummyCtx() -> GLCTX;
    private:
        #if defined(WINDOWS_PLT)
        auto init_opengl_win32()  -> void ;
        #elif defined(LINUX_PLT)
        auto init_opengl_linux()  -> void ;
        #elif defined(WEB_PLT)
        auto init_opengl_web()    -> void ;
        #endif

    private:
        GLCTX m_Context;
        H_SRF m_Surface;
        GLint m_Major;
        GLint m_Minor;
        std::time_t m_CreationTime;

        inline static std::string m_Vendor{};
        inline static std::string m_Renderer{};
        inline static std::vector<std::string> m_Extensions{};
        inline static GLint m_MaxTextureUnits{};
        inline static GLint m_MaxTextureSize{};
        inline static GLint m_MaxTexture3DSize{};
        inline static GLint m_MaxTextureCubeMapSize{};

    public:
        #undef GLFUN
        #ifdef DEBUG
        #   define GLFUN(name)\
            inline static Function<decltype(&gl##name)> name
        #else
        #   define GLFUN(name)\
            inline static decltype(&gl##name) name = Function<decltype(&gl##name)>::default_
        #endif
        GLFUNCS(GLFUN)

        FOR_TEST
};

#undef GLFUN
#ifdef DEBUG
#   define GLFUN(name)\
inline Function<decltype(&gl##name)>& name = OpenGL::name
#else
#   define GLFUN(name)\
inline decltype(&gl##name)& name = OpenGL::name
#endif

GLFUNCS(GLFUN)

} //namespace g