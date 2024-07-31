#pragma once
#include <windows.h>
class OpenGL
{
    public:
        OpenGL(HWND window);
        OpenGL(const OpenGL&) = delete;
        OpenGL operator = (const OpenGL&) = delete;
        ~OpenGL();
    public:
        HDC GetHDC() const;

    private:
    static void init_opengl();
    static void init_opengl_extensions();

    private:
        inline static HWND m_MainWindow;
        inline static HDC m_MainHDC;
        inline static HGLRC m_Context;
        enum {
            WGL_CONTEXT_MAJOR_VERSION_ARB    = 0x2091,
            WGL_CONTEXT_MINOR_VERSION_ARB    = 0x2092,
            WGL_CONTEXT_PROFILE_MASK_ARB     = 0x9126,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001,
            WGL_DRAW_TO_WINDOW_ARB           = 0x2001,
            WGL_ACCELERATION_ARB             = 0x2003,
            WGL_SUPPORT_OPENGL_ARB           = 0x2010,
            WGL_DOUBLE_BUFFER_ARB            = 0x2011,
            WGL_PIXEL_TYPE_ARB               = 0x2013,
            WGL_COLOR_BITS_ARB               = 0x2014,
            WGL_DEPTH_BITS_ARB               = 0x2022,
            WGL_STENCIL_BITS_ARB             = 0x2023,
            WGL_FULL_ACCELERATION_ARB        = 0x2027,
            WGL_TYPE_RGBA_ARB                = 0x202B
        };
};