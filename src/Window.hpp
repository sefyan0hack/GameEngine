#pragma once
#include "Global_H.hpp"
#include <windows.h>

NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

class Window
{
    private:
        Window();
    
    public:
        Window(const Window&) = delete;
        Window(Window&&) = delete;
    
        Window operator = (const Window &) = delete;
        Window operator = (Window &&) = delete;
    
        ~Window();
    
    public:
        static void ProcessMessages();
        HINSTANCE GetHINSTANCE() const;
        HWND GetHWND() const;
        HDC GetHDC() const;
        POINT GetMousePosition() const;
        bool ShouldWindowClose() const;
        static Window & InstanceWindow();
    
    private:
        static void init_opengl_extensions();
        static void init_opengl();
        static LRESULT CALLBACK WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr);
        void _init_helper();
    
    private:
        inline static HINSTANCE m_Instance{};
        inline static HWND m_WindowHandle{};
        inline static HDC m_HDC{};
        inline static HGLRC m_OpenGL_Context{};
    
        // inline static unsigned short S_WindowsCount = 0;
        inline static bool Runing = true;
    
    public:
        inline static long m_Width = 0;
        inline static long m_Height = 0;
    private:
        inline static POINT m_MousePos{0, 0};
    
    private:
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

