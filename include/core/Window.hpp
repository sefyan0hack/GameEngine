#pragma once

#include <core/OpenGL.hpp>
#include <core/OpenGL.hpp>
#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#endif

class CWindow
{
    #if defined(WINDOWS_PLT)
    class WinClass
    {
        private:
            WinClass();
            WinClass(const WinClass&) = delete;
            WinClass(WinClass&&) = delete;

            auto operator = (const WinClass&) -> WinClass = delete ;
            auto operator = (WinClass&&)      -> WinClass = delete ;
        public:
            ~WinClass() = default;
            static auto Instance()  -> WinClass& ;
            static auto Name()      -> const TCHAR* ;
            inline static ATOM m_Windclass = {};
        private:
            inline static const char* m_Name = "singleton WINDOW CLASS";
            inline static WNDCLASSEX m_WinclassEx = {};
    };
    #endif
    
    public:
        CWindow(int Width, int Height, const char* Title);
        CWindow(const CWindow& other);
        CWindow(CWindow&&) = delete;
    
        auto operator = (const CWindow &) -> CWindow = delete ;
        auto operator = (CWindow &&)      -> CWindow = delete ;
    
        ~CWindow();
    
    public:
        auto WindowHandle() const        -> WindHandl ;
        auto DrawContext() const         -> HDC_D ;
        auto Width() const       -> int ;
        auto Height() const      -> int ;
        auto opengl() const          -> std::shared_ptr<gl::OpenGL> ;
        auto Visible() const        -> bool ;
        auto Show()                 -> void;
        auto Hide()                 -> void;
        static auto WindowsCount()  -> unsigned short ;
        static auto ProcessMessages() -> void ;

    private:
        #if defined(WINDOWS_PLT)
        static auto CALLBACK WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)  -> LRESULT ;
        static auto CALLBACK WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT ;
        auto CALLBACK WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)           -> LRESULT ;
        #endif
        #if defined(WEB_PLT)
        static int ResizeHandler(int eventType, const EmscriptenUiEvent* e, void* userData);
        static int KeyHandler(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
        static int MouseHandler(int eventType, const EmscriptenMouseEvent* e, void* userData);
        #endif
        auto _init_helper(int Width, int Height, const char* Title)                         -> void ;
    private:
        WindHandl m_WindowHandle;
        HDC_D m_DrawContext;
        int m_Width;
        int m_Height;
        bool m_Visible;
        std::vector<std::byte> m_RawBuffer;
        std::shared_ptr<gl::OpenGL> m_OpenGl;
        int m_refCount;
    public:
        std::shared_ptr<Keyboard> m_Keyboard;
        std::shared_ptr<Mouse> m_Mouse;
        
    public:
        inline static unsigned short S_WindowsCount = 0;

        FOR_TEST
};