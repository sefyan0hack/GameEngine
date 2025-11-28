#pragma once
#include <utility>
#include <string>
#include <engine_export.h>

#if defined(WINDOWS_PLT)
#include <windows.h>
#undef near
#undef far
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
#include <graphics/gl.hpp>

class EventQueue;

class ENGINE_EXPORT CWindow
{
    friend class APP;

    #if defined(WINDOWS_PLT)
    class WinClass
    {
        private:
            WinClass();
            ~WinClass();
            WinClass(const WinClass&) = delete;
            WinClass(WinClass&&) = delete;

            auto operator = (const WinClass&) -> WinClass& = delete ;
            auto operator = (WinClass&&)      -> WinClass& = delete ;
        public:
            static auto instance()  -> WinClass& ;
            static auto name()      -> const TCHAR* ;
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
        auto handle() const         -> H_WIN ;
        auto surface() const        -> H_SRF ;
        auto width() const          -> int32_t ;
        auto height() const         -> int32_t ;
        auto visible() const        -> bool;
        auto show()                 -> void;
        auto hide()                 -> void;
        auto toggle_fullscreen()     -> void;
        auto swap_buffers() const    -> void;
        auto close()                -> void;
        auto get_title() -> std::string;
        auto set_title(std::string title) -> void;
        auto set_vsync(bool state) -> void;
        auto is_fullscreen() -> bool;

        static auto process_messages([[maybe_unused]] CWindow* self)   -> void ;

    private:
        #if defined(WINDOWS_PLT)
        static auto CALLBACK win_proc_thunk(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)  -> LRESULT ;
        auto CALLBACK win_proc_fun(HWND Winhandle, UINT msg, WPARAM Wpr, LPARAM Lpr)           -> LRESULT ;
        #endif
        #if defined(WEB_PLT)
        static auto resize_callback(int32_t eventType, const EmscriptenUiEvent* e, void* userData)    -> EM_BOOL;
        static auto keyboard_callback(int32_t eventType, const EmscriptenKeyboardEvent* e, void* userData) -> EM_BOOL;
        static auto mouse_callback(int32_t eventType, const EmscriptenMouseEvent* e, void* userData)  -> EM_BOOL;
        static auto touch_callback(int32_t eventType, const EmscriptenTouchEvent* e, void* userData)  -> EM_BOOL;

        auto register_event_callbacks() -> void;
        #endif

        auto new_window(int32_t Width, int32_t Height, const char* Title)       -> std::pair<H_WIN, H_SRF> ;

    private:
        H_WIN m_Handle;
        H_SRF m_Surface;
        int32_t m_Width, m_Height;
        bool m_Visible, m_FullScreen;
        EventQueue& m_EventQueue;

};