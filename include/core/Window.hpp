#pragma once
#include <memory>
#include <vector>
#include <core/OpenGL.hpp>
#include <windows.h>
#include <tchar.h>
#include <core/gl.h>
#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>

class Window
{
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
            inline static const TCHAR* m_Name = _T("singleton WINDOW CLASS");
            inline static WNDCLASSEX m_WinclassEx = {};
    };
    
    public:
        Window(int Width, int Height, const TCHAR* Title);
        Window(const Window&) = delete;
        Window(Window&&) = delete;
    
        auto operator = (const Window &) -> Window = delete ;
        auto operator = (Window &&)      -> Window = delete ;
    
        ~Window();
    
    public:
        auto GetHINSTANCE() const   -> HINSTANCE ;
        auto GetHWND() const        -> HWND ;
        auto GetHDC() const         -> HDC ;
        auto GetWidth() const       -> int ;
        auto GetHeight() const      -> int ;
        auto GetGL() const          -> OpenGL* ;
        auto Visible() const        -> bool ;
        static auto WindowsCount()  -> unsigned short ;
        static auto ProcessMessages() -> void ;

    private:
        static auto CALLBACK WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)  -> LRESULT ;
        static auto CALLBACK WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT ;
        auto CALLBACK WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)           -> LRESULT ;
        auto _init_helper(int Width, int Height, const TCHAR* Title)                         -> void ;
    private:
        HINSTANCE m_Instance;
        HWND m_WindowHandle;
        HDC m_HDC;
        int m_Width;
        int m_Height;
        bool m_Visible;
        std::vector<BYTE> rawBuffer;
        std::unique_ptr<OpenGL> m_OpenGl;
    public:
	    Keyboard kbd;
        Mouse mouse;
        
    public:
        inline static unsigned short S_WindowsCount = 0;
};

