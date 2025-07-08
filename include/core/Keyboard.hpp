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
		Key code;
	};
public:

	Keyboard();
	Keyboard( const Keyboard& ) = delete;
	auto operator=( const Keyboard& ) -> Keyboard& = delete;
	auto ReadKey() noexcept 								 	-> std::optional<Event> ;
	auto ReadChar() noexcept 								 	-> std::optional<unsigned char> ;

	auto IsKeyPressed(Key key ) const noexcept 	    -> bool ;
	auto IsKeyReleased(Key key) const noexcept      -> bool ;
	auto IsKeyDown(Key key) const noexcept 		    -> bool ;
	auto IsKeyUp(Key key) const noexcept 			-> bool ;

	auto FlushKey() noexcept 								 	-> void ;
	auto FlushChar() noexcept 									-> void ;
	auto Flush() noexcept 										-> void ;
	auto EnableAutorepeat() noexcept 							-> void ;
	auto DisableAutorepeat() noexcept 							-> void ;
	auto AutorepeatIsEnabled() const noexcept 					-> bool ;
	auto UpdatePrevState() noexcept 							-> void ;

    static auto ToNative(Key key) -> uint64_t;
    static auto FromNative(uint64_t key) -> Key;

private:
	auto OnKeyPressed( Key key ) noexcept  		-> void ;
	auto OnKeyReleased( Key key ) noexcept 		-> void ;
	auto OnChar( unsigned char character ) noexcept 			-> void ;
	auto ClearState() noexcept 									-> void ;
	template<typename Container>
	requires requires(Container c) { c.size(); c.pop(); }
	static auto TrimBuffer( Container& buffer ) noexcept	-> void;

    constexpr static std::pair<uint64_t, Key> key_to_pair( Key key ){
        return std::pair<uint64_t, Key>(ToNative(key), key);
    }

private:
	static constexpr uint32_t bufferSize = 16u;
	bool autorepeatEnabled = true;

	std::bitset<std::to_underlying(Key::Unknown)> keystates;
	std::bitset<std::to_underlying(Key::Unknown)> prevKeyStates;
	std::queue<Event> keybuffer;
	std::queue<unsigned char> charbuffer;
    inline static std::map<uint64_t, Key> KeyMaps {
        // Alphabet
        key_to_pair(Key::A),
        key_to_pair(Key::B),
        key_to_pair(Key::C),
        key_to_pair(Key::D),
        key_to_pair(Key::E),
        key_to_pair(Key::F),
        key_to_pair(Key::G),
        key_to_pair(Key::H),
        key_to_pair(Key::I),
        key_to_pair(Key::J),
        key_to_pair(Key::K),
        key_to_pair(Key::L),
        key_to_pair(Key::M),
        key_to_pair(Key::N),
        key_to_pair(Key::O),
        key_to_pair(Key::P),
        key_to_pair(Key::Q),
        key_to_pair(Key::R),
        key_to_pair(Key::S),
        key_to_pair(Key::T),
        key_to_pair(Key::U),
        key_to_pair(Key::V),
        key_to_pair(Key::W),
        key_to_pair(Key::X),
        key_to_pair(Key::Y),
        key_to_pair(Key::Z),

        // Numbers
        key_to_pair(Key::Num0),
        key_to_pair(Key::Num1),
        key_to_pair(Key::Num2),
        key_to_pair(Key::Num3),
        key_to_pair(Key::Num4),
        key_to_pair(Key::Num5),
        key_to_pair(Key::Num6),
        key_to_pair(Key::Num7),
        key_to_pair(Key::Num8),
        key_to_pair(Key::Num9),
        //FXX
        key_to_pair(Key::F1),
        key_to_pair(Key::F2),
        key_to_pair(Key::F3),
        key_to_pair(Key::F4),
        key_to_pair(Key::F5),
        key_to_pair(Key::F6),
        key_to_pair(Key::F7),
        key_to_pair(Key::F8),
        key_to_pair(Key::F9),
        key_to_pair(Key::F10),
        key_to_pair(Key::F11),
        key_to_pair(Key::F12),
        key_to_pair(Key::F13),
        key_to_pair(Key::F14),
        key_to_pair(Key::F15),
        key_to_pair(Key::F16),
        key_to_pair(Key::F17),
        key_to_pair(Key::F18),
        key_to_pair(Key::F19),
        key_to_pair(Key::F20),
        key_to_pair(Key::F21),
        key_to_pair(Key::F22),
        key_to_pair(Key::F23),
        key_to_pair(Key::F24),

        // Specials
        key_to_pair(Key::Escape),
        key_to_pair(Key::Enter),
        key_to_pair(Key::Tab),
        key_to_pair(Key::Backspace),
        key_to_pair(Key::Space),
        key_to_pair(Key::CapsLock),
        key_to_pair(Key::PrintScreen),
        key_to_pair(Key::ScrollLock),
        key_to_pair(Key::Pause),

        // Navigations
        key_to_pair(Key::Insert),
        key_to_pair(Key::Home),
        key_to_pair(Key::PageUp),
        key_to_pair(Key::Delete),
        key_to_pair(Key::End),
        key_to_pair(Key::PageDown),
        
        // Arrow
        key_to_pair(Key::Left),
        key_to_pair(Key::Right),
        key_to_pair(Key::Up),
        key_to_pair(Key::Down),

        // Modifiers
        key_to_pair(Key::LeftShift),
        key_to_pair(Key::LeftControl),
        key_to_pair(Key::LeftAlt),
        key_to_pair(Key::LeftSuper),
        key_to_pair(Key::RightShift),
        key_to_pair(Key::RightControl),
        key_to_pair(Key::RightAlt),
        key_to_pair(Key::RightSuper),

        // Numpad
        key_to_pair(Key::NumPad0),
        key_to_pair(Key::NumPad1),
        key_to_pair(Key::NumPad2),
        key_to_pair(Key::NumPad3),
        key_to_pair(Key::NumPad4),
        key_to_pair(Key::NumPad5),
        key_to_pair(Key::NumPad6),
        key_to_pair(Key::NumPad7),
        key_to_pair(Key::NumPad8),
        key_to_pair(Key::NumPad9),
        key_to_pair(Key::NumPadDecimal),
        key_to_pair(Key::NumPadDivide),
        key_to_pair(Key::NumPadMultiply),
        key_to_pair(Key::NumPadSubtract),
        key_to_pair(Key::NumPadAdd),
        key_to_pair(Key::NumPadEnter),
        key_to_pair(Key::NumPadEqual),

        // Punctuation
        key_to_pair(Key::Comma),
        key_to_pair(Key::Period),
        key_to_pair(Key::Semicolon),
        key_to_pair(Key::Apostrophe),
        key_to_pair(Key::BracketLeft),
        key_to_pair(Key::BracketRight),
        key_to_pair(Key::Backslash),
        key_to_pair(Key::Slash),
        key_to_pair(Key::GraveAccent),
        key_to_pair(Key::Minus),
        key_to_pair(Key::Equal),
        
        // Web-specific
        #if defined(WEB_PLT)
        // key_to_pair(Key::BrowserBack),
        // key_to_pair(Key::BrowserForward),
        // key_to_pair(Key::BrowserRefresh),
        // key_to_pair(Key::BrowserStop),
        // key_to_pair(Key::BrowserSearch),
        // key_to_pair(Key::BrowserFavorites),
        // key_to_pair(Key::BrowserHome),
        // key_to_pair(Key::VolumeMute),
        // key_to_pair(Key::VolumeDown),
        // key_to_pair(Key::VolumeUp),
        // key_to_pair(Key::MediaNext),
        // key_to_pair(Key::MediaPrevious),
        // key_to_pair(Key::MediaStop),
        // key_to_pair(Key::MediaPlayPause),
        #endif
    };

	FOR_TEST
};