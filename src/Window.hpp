#pragma once
#include "Global_H.hpp"
#include <windows.h>
#include <memory>
#include <vector>
#include "OpenGL.hpp"

NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

#include "Keyboard.hpp"
#include "Mouse.hpp"

class Window
{
    class WinClass
    {
        private:
            WinClass();
            WinClass(const WinClass&) = delete;
            WinClass operator = (const WinClass&) = delete;

            WinClass(WinClass&&) = delete;
            WinClass operator = (WinClass&&) = delete;
        public:
            ~WinClass();
            static WinClass & Instance();
            static const char* Name();
        private:
            inline static const char* m_Name = "singleton WINDOW CLASS";
            inline static WNDCLASSEXA m_Winclass = {};
    };
    
    public:
        Window(int Width, int Height, const char* Title);
        Window(const Window&) = delete;
        Window(Window&&) = delete;
    
        Window operator = (const Window &) = delete;
        Window operator = (Window &&) = delete;
    
        ~Window();
    
    public:
        HINSTANCE GetHINSTANCE() const;
        HWND GetHWND() const;
        HDC GetHDC() const;
        int GetWidth() const;
        int GetHeight() const;
        OpenGL* GetGL() const;
        bool Visible() const;
        static unsigned short WindowsCount();

    public:
        static void ProcessMessages();
    private:
        static LRESULT CALLBACK WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr);
        static LRESULT CALLBACK WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr);
        LRESULT CALLBACK WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr);
        void _init_helper(int Width, int Height, const char* Title);
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

