#pragma once
#include <utility>

#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
#include "gl.h"

class EventQueue;

class CWindow
{
    friend class APP;

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
        struct ResizeEvent { int32_t width, height; };
        struct LoseFocusEvent { const CWindow* window; };
        struct SetFocusEvent  { const CWindow* window; };
        struct QuitEvent {};

    private:
        CWindow(int32_t Width, int32_t Height, const char* Title, EventQueue& Queue) noexcept;

    public:
        ~CWindow();

        CWindow(const CWindow& other) = delete;
        auto operator = (const CWindow &) -> CWindow& = delete ;

        CWindow(CWindow&&) noexcept= delete;
        auto operator = (CWindow &&) noexcept -> CWindow& = delete ;

    public:
        auto Handle() const         -> H_WIN ;
        auto Surface() const        -> H_SRF ;
        auto Width() const          -> int32_t ;
        auto Height() const         -> int32_t ;
        auto Visible() const        -> bool;
        auto Show()                 -> void;
        auto Hide()                 -> void;
        auto ToggleFullScreen()     -> void;
        auto SwapBuffers() const    -> void;
        auto Close()                -> void;
        auto GetTitle() -> std::string;
        auto SetTitle(std::string title) -> void;
        static auto ProcessMessages([[maybe_unused]] CWindow* self)   -> void ;

    private:
        #if defined(WINDOWS_PLT)
        static auto CALLBACK WinProcThunk(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)  -> LRESULT ;
        auto CALLBACK WinProcFun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)           -> LRESULT ;
        #endif
        #if defined(WEB_PLT)
        static auto ResizeCallback(int32_t eventType, const EmscriptenUiEvent* e, void* userData)    -> EM_BOOL;
        static auto KeyboardCallback(int32_t eventType, const EmscriptenKeyboardEvent* e, void* userData) -> EM_BOOL;
        static auto MouseCallback(int32_t eventType, const EmscriptenMouseEvent* e, void* userData)  -> EM_BOOL;
        static auto TouchCallback(int32_t eventType, const EmscriptenTouchEvent* e, void* userData)  -> EM_BOOL;

        auto RegisterEventCallbacks() -> void;
        #endif

        auto new_window(int32_t Width, int32_t Height, const char* Title)       -> std::pair<H_WIN, H_SRF> ;

    private:
        H_WIN m_Handle;
        H_SRF m_Surface;
        int32_t m_Width, m_Height;
        bool m_Visible, m_FullScreen;
        EventQueue& m_EventQueue;

        FOR_TEST
};