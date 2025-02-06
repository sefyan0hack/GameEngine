#pragma once

#include <windows.h>
#include <core/gl.h>

class OpenGL
{
    public:
        explicit OpenGL(HWND window);
        OpenGL(const OpenGL&) = delete;
        auto operator = (const OpenGL&) -> OpenGL = delete;
        ~OpenGL();
    public:
        auto GetHDC() const -> HDC;

        static auto MajorV() -> GLint;
        static auto MinorV() -> GLint;

        inline static LPCSTR OPENGL_MODULE_NAME {"opengl32.dll"};
    private:
        auto init_opengl()             -> void ;

    private:
        HWND m_MainWindow;
        HDC m_MainHDC;
        HGLRC m_Context;

        inline static GLint vMajor;
        inline static GLint vMinor;
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
        };
};