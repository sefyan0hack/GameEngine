#pragma once
#include <ranges>
#include <bitset>
#include <map>
#include <engine_export.h>

#if defined(WEB_PLT)
// Browser control keys (DOM Level 3)
constexpr uint32_t DOM_VK_BROWSER_BACK = 166;
constexpr uint32_t DOM_VK_BROWSER_FORWARD = 167;
constexpr uint32_t DOM_VK_BROWSER_REFRESH = 168;
constexpr uint32_t DOM_VK_BROWSER_STOP = 169;
constexpr uint32_t DOM_VK_BROWSER_SEARCH = 170;
constexpr uint32_t DOM_VK_BROWSER_FAVORITES = 171;
constexpr uint32_t DOM_VK_BROWSER_HOME = 172;

// Media control keys (DOM Level 3)
constexpr uint32_t DOM_VK_MEDIA_TRACK_NEXT = 176;
constexpr uint32_t DOM_VK_MEDIA_TRACK_PREVIOUS = 177;
constexpr uint32_t DOM_VK_MEDIA_STOP = 178;
constexpr uint32_t DOM_VK_MEDIA_PLAY_PAUSE = 179;
#endif

enum class Key : uint8_t {

    // Alphanumeric keys (A-Z, 0-9)
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

    // Special keys
    Escape, Enter, Tab, Backspace, Space, 
    CapsLock, PrintScreen, ScrollLock, Pause,

    // Navigation keys
    Insert, Home, PageUp,
    Delete, End, PageDown,

    // Arrow keys
    Right, Left, Down, Up,

    // Modifier keys
    LeftShift, LeftControl, LeftAlt, LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper,

    // Numpad keys
    NumPad0, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6,
    NumPad7, NumPad8, NumPad9, NumPadDecimal, NumPadDivide,
    NumPadMultiply, NumPadSubtract, NumPadAdd, NumPadEnter, NumPadEqual,

    // Punctuation keys
    Comma, Period, Semicolon, Apostrophe, BracketLeft, BracketRight,
    Backslash, Slash, GraveAccent, Minus, Equal,

    #if defined(WEB_PLT)
    // Web-specific keys
    BrowserBack, BrowserForward, BrowserRefresh, BrowserStop, 
    BrowserSearch, BrowserFavorites, BrowserHome, VolumeMute, VolumeDown,
    VolumeUp, MediaNext, MediaPrevious, MediaStop, MediaPlayPause,
    #endif

    Unknown
};

class ENGINE_API Keyboard
{
	friend class APP;
public:

    struct KeyDownEvent { Key key; };
    struct KeyUpEvent { Key key; };

private:
    Keyboard() = default;

public:

	Keyboard(const Keyboard&) = delete;
	auto operator=(const Keyboard&) -> Keyboard& = delete;

	Keyboard(Keyboard&&) = delete;
	auto operator=(Keyboard&&) -> Keyboard& = delete;

	auto is_pressed(Key key) const noexcept      -> bool ;
	auto is_released(Key key) const noexcept      -> bool ;
	auto is_down(Key key) const noexcept          -> bool ;
	auto is_up(Key key) const noexcept            -> bool ;

    static auto to_native(Key key) -> uint32_t;
    static auto from_native(uint32_t key) -> Key;

private:
	auto on_key_down(Key key) noexcept  		-> void ;
	auto on_key_up(Key key) noexcept 		-> void ;

	auto clear_state() noexcept                  -> void ;
    auto save_prev_state() noexcept                   -> void ;

private:
    static constexpr size_t KeyCount = std::to_underlying(Key::Unknown) + 1 ;

	std::bitset<KeyCount> m_CurrKeyState;
    std::bitset<KeyCount> m_PrevKeyState;

    inline static auto KeyMaps = [](){
        std::map<uint32_t, Key> r;

        for(auto k :
            std::views::iota(std::to_underlying(Key::A), std::to_underlying(Key::Unknown)) |
            std::views::transform([](auto&& a){ return static_cast<Key>(a); })
        ){
            r.try_emplace(to_native(k), k);
        }

        return r;
    }();

	
};