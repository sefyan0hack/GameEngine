#pragma once

#include <core/OpenGL.hpp>
#include <core/OpenGL.hpp>
#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>
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

            auto operator = (const WinClass&) -> WinClass& = delete ;
            auto operator = (WinClass&&)      -> WinClass& = delete ;
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
        CWindow(int32_t Width, int32_t Height, const char* Title, bool withopengl = true);
        CWindow(const CWindow& other);
        CWindow(CWindow&&) = delete;
    
        auto operator = (const CWindow &) -> CWindow& = delete ;
        auto operator = (CWindow &&)      -> CWindow& = delete ;
    
        ~CWindow();
    
    public:
        auto Handle() const              -> WindHandl ;
        auto DrawContext() const         -> HDC_D ;
        auto Width() const       -> int32_t ;
        auto Height() const      -> int32_t ;
        auto opengl() const          -> std::shared_ptr<gl::OpenGL> ;
        auto Visible() const        -> bool ;
        auto Show()                 -> void;
        auto Hide()                 -> void;
        auto ToggleFullScreen()     -> void;
        static auto WindowsCount()  -> unsigned short ;
        static auto ProcessMessages(WindHandl wnhd = WindHandl{}, HDC dctx = HDC{})   -> void ;
        static auto WindowShouldClose() -> bool ;


    private:
        #if defined(WINDOWS_PLT)
        static auto CALLBACK WinProcSetup(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)  -> LRESULT ;
        static auto CALLBACK WinProcSetup2(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr) -> LRESULT ;
        auto CALLBACK WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)           -> LRESULT ;
        #endif
        #if defined(WEB_PLT)
        static auto ResizeHandler(int32_t eventType, const EmscriptenUiEvent* e, void* userData)    -> EM_BOOL;
        static auto KeyHandler(int32_t eventType, const EmscriptenKeyboardEvent* e, void* userData) -> EM_BOOL;
        static auto MouseHandler(int32_t eventType, const EmscriptenMouseEvent* e, void* userData)  -> EM_BOOL;
        #endif
        auto _init_helper(int32_t Width, int32_t Height, const char* Title)                         -> void ;
    private:
        WindHandl m_WindowHandle;
        HDC_D m_DrawContext;
        int32_t m_Width;
        int32_t m_Height;
        bool m_Visible;
        std::vector<std::byte> m_RawBuffer;
        std::shared_ptr<gl::OpenGL> m_OpenGl;
        int32_t m_refCount;
    public:
        std::shared_ptr<Keyboard> m_Keyboard;
        std::shared_ptr<Mouse> m_Mouse;
        
    public:
        inline static unsigned short S_WindowsCount = 0;

        FOR_TEST
};