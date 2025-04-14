#pragma once

#include <core/OpenGL.hpp>
#include <core/gl.h>
#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>
#ifdef __linux__
#include <X11/Xlib.h>

class CWindow
{ 
    public:
        CWindow(int Width, int Height, const char* Title);
        CWindow(const CWindow& other);
        CWindow(CWindow&&) = delete;
    
        auto operator = (const CWindow &) -> CWindow = delete ;
        auto operator = (CWindow &&)      -> CWindow = delete ;
    
        ~CWindow();
    
    public:
        // auto Hinstance() const   -> HINSTANCE ;
        // auto WindowHandle() const        -> HWND ;
        // auto DrawContext() const         -> HDC ;
        auto Width() const       -> int ;
        auto Height() const      -> int ;
        auto opengl() const          -> std::shared_ptr<OpenGL> ;
        auto Visible() const        -> bool ;
        static auto WindowsCount()  -> unsigned short ;
        auto ProcessMessages() -> void ;

    private:
        auto _init_helper(int Width, int Height, const char* Title)                         -> void ;
    private:
        Display* m_Display;
        Window m_WindowHandle;
        XEvent  m_Event;
        int m_Width;
        int m_Height;
        bool m_Visible;
        std::vector<std::byte> m_RawBuffer;
        std::shared_ptr<OpenGL> m_OpenGl;
    public:
        std::shared_ptr<Keyboard> m_Keyboard;
        std::shared_ptr<Mouse> m_Mouse;
        
    public:
        inline static unsigned short S_WindowsCount = 0;
};

#endif //__linux__
