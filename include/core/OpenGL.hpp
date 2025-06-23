#pragma once

#include <core/gl.h>

namespace gl {
    #if defined(WEB_PLT)
    constexpr const char* OPENGL_MODULE_NAME {"WebGL"};
    #else
    constexpr const char* OPENGL_MODULE_NAME {OPENGL_LIB};
    #endif

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
        explicit OpenGL(WindHandl window, HDC_D drawContext);
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
        auto DrawContext() const -> HDC_D;
        auto MajorV() const -> GLint;
        auto MinorV() const -> GLint;
        auto isValid() const -> bool;
        auto CreationTime() const -> std::time_t;

        static auto Vendor() -> std::string;
        static auto Renderer() -> std::string;
        static auto Extensions() -> std::vector<std::string>;
        static auto MaxTextureUnits() -> GLint;
        
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
        HDC_D m_DrawContext;
        GLint m_Major;
        GLint m_Minor;
        std::time_t m_CreationTime;

        inline static std::string m_Vendor{};
        inline static std::string m_Renderer{};
        inline static std::vector<std::string> m_Extensions{};
        inline static GLint m_MaxTextureUnits{};

        enum {
            CONTEXT_MAJOR_VERSION_ARB     = 0x2091,
            CONTEXT_MINOR_VERSION_ARB     = 0x2092,
            CONTEXT_FLAGS_ARB             = 0x2094,
            CONTEXT_CORE_PROFILE_BIT_ARB  = 0x00000001,
            CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002,
            CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB  = 0x8256,
            CONTEXT_PROFILE_MASK_ARB      = 0x9126,
            // GL_ALL_ATTRIB_BITS               = 0x000fffff,

            #if defined(WINDOWS_PLT)
            WGL_DRAW_TO_WINDOW_ARB           = 0x2001,
            WGL_ACCELERATION_ARB             = 0x2003,
            WGL_SUPPORT_OPENGL_ARB           = 0x2010,
            WGL_DOUBLE_BUFFER_ARB            = 0x2011,
            WGL_PIXEL_TYPE_ARB               = 0x2013,
            WGL_COLOR_BITS_ARB               = 0x2014,
            WGL_DEPTH_BITS_ARB               = 0x2022,
            WGL_STENCIL_BITS_ARB             = 0x2023,
            WGL_FULL_ACCELERATION_ARB        = 0x2027,
            WGL_TYPE_RGBA_ARB                = 0x202B,
            ERROR_INVALID_VERSION_ARB        = 0x2095,
            ERROR_INVALID_PROFILE_ARB        = 0x2096,
            #elif defined(LINUX_PLT)
            // GLX_NONE        			    = 0x8000,
            // GLX_DRAWABLE_TYPE               = 0x8010,
            // GLX_RENDER_TYPE                 = 0x8011,
            // GLX_X_RENDERABLE                = 0x8012,
            // GLX_DOUBLEBUFFER                = 5,
            // GLX_RED_SIZE		            = 8,
            // GLX_GREEN_SIZE		            = 9,
            // GLX_BLUE_SIZE		            = 10,
            // GLX_ALPHA_SIZE		            = 11,
            // GLX_DEPTH_SIZE		            = 12,
            #endif 
        };
    public:
        #undef GLFUN
        #ifdef DEBUG
        #   define GLFUN(type, name)\
            inline static Function<type> name
        #else
        #   define GLFUN(type, name)\
            inline static type name = Function<type>::default_
        #endif
        GLFUNCS(GLFUN)

        FOR_TEST
};

#undef GLFUN
#ifdef DEBUG
#   define GLFUN(type, name)\
inline Function<type>& name = OpenGL::name
#else
#   define GLFUN(type, name)\
inline type& name = OpenGL::name
#endif

GLFUNCS(GLFUN)

} //namespace gl