#pragma once

enum class Key : uint16_t {

    // Alphanumeric keys (A-Z, 0-9)
    A = 'A', B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0 = '0', Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // Function keys
    F1 = 0x0100, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

    // Special keys
    Escape = 0x0200,
    Enter,
    Tab,
    Backspace,
    Space,
    CapsLock,
    PrintScreen,
    ScrollLock,
    Pause,

    // Navigation keys
    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,

    // Arrow keys
    Right = 0x0300,
    Left,
    Down,
    Up,

    // Modifier keys
    LeftShift = 0x0400,
    LeftControl,
    LeftAlt,
    LeftSuper,  // Windows/Command key
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,

    // Numpad keys
    NumPad0 = 0x0500,
    NumPad1,
    NumPad2,
    NumPad3,
    NumPad4,
    NumPad5,
    NumPad6,
    NumPad7,
    NumPad8,
    NumPad9,
    NumPadDecimal,
    NumPadDivide,
    NumPadMultiply,
    NumPadSubtract,
    NumPadAdd,
    NumPadEnter,
    NumPadEqual,

    // Punctuation keys
    Comma = ',',
    Period = '.',
    Semicolon = ';',
    Apostrophe = '\'',
    BracketLeft = '[',
    BracketRight = ']',
    Backslash = '\\',
    Slash = '/',
    GraveAccent = '`',  // Tilde ~
    Minus = '-',
    Equal = '=',

    #if defined(WEB_PLT)
    // Web-specific keys
    BrowserBack = 0x0600,
    BrowserForward,
    BrowserRefresh,
    BrowserStop,
    BrowserSearch,
    BrowserFavorites,
    BrowserHome,
    VolumeMute,
    VolumeDown,
    VolumeUp,
    MediaNext,
    MediaPrevious,
    MediaStop,
    MediaPlayPause,
    #endif
};

namespace KeyMapper {
    #if defined(WINDOWS_PLT)
        #include <windows.h>
        uint32_t ToNative(Key key);
    #elif defined(LINUX_PLT)
        #include <X11/X.h>
        #include <X11/keysym.h>
        KeySym ToNative(Key key);
    #elif defined(WEB_PLT)
        #include <emscripten/html5.h>
        const char* ToNative(Key key);
    #endif
}

class CWindow;  // Forward declaration if necessary

class Keyboard
{
	friend class CWindow;
private:
	class Event
	{
	public:
		enum class Type { Press, Release };
	public:
		Event( Type type, Key code ) noexcept;
		auto IsPress() const noexcept 	-> bool ;
		auto IsRelease() const noexcept -> bool ;
		auto Code() const noexcept 	-> Key;
    private:
		Type type;
		Key  code;
	};
public:
	Keyboard();
	Keyboard( const Keyboard& ) = delete;
	auto operator=( const Keyboard& ) -> Keyboard& = delete;
	auto ReadKey() noexcept 								 	-> std::optional<Event> ;
	auto ReadChar() noexcept 								 	-> std::optional<unsigned char> ;

	auto IsKeyPressed(Key key ) const noexcept 	-> bool ;
	auto IsKeyReleased(Key key) const noexcept    -> bool ;
	auto IsKeyDown(Key key) const noexcept 		-> bool ;
	auto IsKeyUp(Key key) const noexcept 			-> bool ;

	auto FlushKey() noexcept 								 	-> void ;
	auto FlushChar() noexcept 									-> void ;
	auto Flush() noexcept 										-> void ;
	auto EnableAutorepeat() noexcept 							-> void ;
	auto DisableAutorepeat() noexcept 							-> void ;
	auto AutorepeatIsEnabled() const noexcept 					-> bool ;
	auto UpdatePrevState() noexcept 							-> void;

private:
	auto OnKeyPressed( Key key ) noexcept  		-> void ;
	auto OnKeyReleased( Key key ) noexcept 		-> void ;
	auto OnChar( unsigned char character ) noexcept 			-> void ;
	auto ClearState() noexcept 									-> void ;
	template<typename Container>
	requires requires(Container c) { c.size(); c.pop(); }
	static auto TrimBuffer( Container& buffer ) noexcept	-> void;

private:
	static constexpr uint32_t nKeys = 0x0700; // Covers up to 0x06FF web keys
	static constexpr uint32_t bufferSize = 16u;
	bool autorepeatEnabled = true;

	std::bitset<nKeys> keystates;
	std::bitset<nKeys> prevKeyStates;
	std::queue<Event> keybuffer;
	std::queue<unsigned char> charbuffer;

	FOR_TEST
};