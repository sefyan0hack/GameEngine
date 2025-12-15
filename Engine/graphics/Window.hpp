#pragma once
#include <utility>
#include <string>
#include <engine_export.h>
#include "gl.hpp"

class ENGINE_EXPORT CWindow
{
    public:
        struct ResizeEvent { int32_t width, height; };
        struct LoseFocusEvent { const CWindow* window; };
        struct SetFocusEvent  { const CWindow* window; };
        struct QuitEvent {};

        CWindow(int32_t Width, int32_t Height, const char* Title) noexcept;

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
        auto resize(int32_t width, int32_t height) -> void;

        auto process_messages()   -> void ;
        auto message_box(const char* title, const char* body) -> bool;

    private:
        auto new_window(int32_t Width, int32_t Height, const char* Title)       -> std::pair<H_WIN, H_SRF> ;

    private:
        H_WIN m_Handle;
        H_SRF m_Surface;
        int32_t m_Width, m_Height;
        bool m_Visible, m_FullScreen;
};