#pragma once
#include <ranges>
#include <bitset>
#include <map>
#include <utility>
#include <engine_export.h>

enum class Key {

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

    #if defined(WEB_PLT) // Web-specific keys
    BrowserBack, BrowserForward, BrowserRefresh, BrowserStop, 
    BrowserSearch, BrowserFavorites, BrowserHome,
    #endif

    #if defined(ANDROID_PLT) // Android-specific keys
    Back, Menu, Search,
    #endif
    
    #if defined(WEB_PLT) || defined(ANDROID_PLT)  // (Android/web)-specific keys
    VolumeMute, VolumeDown, VolumeUp, MediaNext, MediaPrevious, MediaStop, MediaPlayPause,
    #endif

    Unknown
};

class ENGINE_EXPORT Keyboard
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

private:
	auto on_key_down(Key key) noexcept  		-> void ;
	auto on_key_up(Key key) noexcept 		-> void ;

	auto clear_state() noexcept                  -> void ;
    auto save_prev_state() noexcept                   -> void ;

private:
    static constexpr size_t KeyCount = std::to_underlying(Key::Unknown) + 1 ;

	std::bitset<KeyCount> m_CurrKeyState;
    std::bitset<KeyCount> m_PrevKeyState;
};