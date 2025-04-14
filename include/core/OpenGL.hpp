#pragma once

#include <core/gl.h>

#ifdef _WIN32
#include <windows.h>
using WindHandl = HWND;
using HDC_D     = HDC;
using GLCTX     = HGLRC;
auto __GetProcAddress(LPCSTR module, const char* name) -> void*;
auto rsgl(const char* name) -> void*;
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifndef GL_GLEXT_PROTOTYPES
#undef GL_GLEXT_PROTOTYPES
#endif
#include <GL/gl.h>
#include <GL/glx.h>
using WindHandl = Window;
using HDC_D     = Display;
using GLCTX     = GLXContext;
#endif

class OpenGL
{
    public:
        explicit OpenGL(WindHandl window);
        OpenGL(const OpenGL& other);
        OpenGL(OpenGL&& other) noexcept;
        ~OpenGL();
        auto operator = (const OpenGL& other) -> OpenGL;
        auto operator = (OpenGL&& other)  noexcept -> OpenGL;
        auto operator == (const OpenGL& other) const -> bool;
        auto operator != (const OpenGL& other) const -> bool;
        operator bool () const;

    public:
        auto DrawContext() const -> HDC_D;
        auto MajorV() const -> GLint;
        auto MinorV() const -> GLint;
        auto isValid() const -> bool;
        auto CreationTime() const -> std::time_t;
        auto isDebugable() const -> bool;

        static auto Vendor() -> std::string;
        static auto Renderer() -> std::string;
        static auto GlslVersions() -> std::vector<std::string>;
        static auto Extensions() -> std::vector<std::string>;
        static auto MaxTextureUnits() -> GLint;

    private:
        #ifdef _WIN32
        friend auto __GetProcAddress(LPCSTR module, const char* name) -> void*;
        friend auto rsgl(const char* name) -> void*;
        inline static LPCSTR OPENGL_MODULE_NAME {"opengl32.dll"};    
        auto init_opengl_win32()              -> void ;
        #elif defined(__linux__)
        auto init_opengl_linux(Window window) -> void ;
        #endif //_WIN32

    private:
        GLCTX m_Context;
        HDC_D m_MainHDC;
        GLint m_Major;
        GLint m_Minor;
        std::time_t m_CreationTime;
        bool m_Debug;

        inline static std::string m_Vendor;
        inline static std::string m_Renderer;
        inline static std::vector<std::string> m_GlslVersions;
        inline static std::vector<std::string> m_Extensions;
        inline static GLint m_MaxTextureUnits;

        enum {
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
            WGL_CONTEXT_MAJOR_VERSION_ARB    = 0x2091,
            WGL_CONTEXT_MINOR_VERSION_ARB    = 0x2092,
            WGL_CONTEXT_FLAGS_ARB            = 0x2094,
            WGL_CONTEXT_PROFILE_MASK_ARB     = 0x9126,
            ERROR_INVALID_VERSION_ARB        = 0x2095,
            ERROR_INVALID_PROFILE_ARB        = 0x2096,
            WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB = 0x8256,
            GL_ALL_ATTRIB_BITS               = 0x000fffff,
        };
};