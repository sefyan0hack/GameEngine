#pragma once

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

class Keyboard
{
	friend class APP;
public:
    enum class Action { Press, Release };
	
    struct Event
	{
		Action action;
        Key key;
	};

public:

	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	auto operator=(const Keyboard&) -> Keyboard& = delete;

	auto IsKeyPressed(Key key) const noexcept      -> bool ;
	auto IsKeyReleased(Key key) const noexcept      -> bool ;
	auto IsKeyDown(Key key) const noexcept          -> bool ;
	auto IsKeyUp(Key key) const noexcept            -> bool ;

    static auto ToNative(Key key) -> uint64_t;
    static auto FromNative(uint64_t key) -> Key;

private:
	auto OnKeyPressed(Key key) noexcept  		-> void ;
	auto OnKeyReleased(Key key) noexcept 		-> void ;

	auto ClearState() noexcept                  -> void ;
    auto SavePrevState() noexcept                   -> void ;

private:
    static constexpr size_t KeyCount = std::to_underlying(Key::Unknown) + 1 ;

	std::bitset<KeyCount> m_CurrKeyState;
    std::bitset<KeyCount> m_PrevKeyState;

    inline static auto KeyMaps = [](){
        std::map<uint64_t, Key> r;

        for(auto k :
            std::views::iota(std::to_underlying(Key::A), std::to_underlying(Key::Unknown)) |
            std::views::transform([](auto&& a){ return static_cast<Key>(a); })
        ){
            r.try_emplace(ToNative(k), k);
        }

        return r;
    }();

	FOR_TEST
};