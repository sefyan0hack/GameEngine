#include <core/Keyboard.hpp>
// Event/////////////////////////////////////////////////
Keyboard::Event::Event( Type type, Key code ) noexcept
    			: type( type )
    			, code( code )
    		{}

auto Keyboard::Event::IsPress() const noexcept -> bool
{
	return type == Type::Press;
}
auto Keyboard::Event::IsRelease() const noexcept -> bool
{
	return type == Type::Release;
}
auto Keyboard::Event::Code() const noexcept -> Key
{
	return code;
}
/////////////////////////////////////////////////////////

//KeyBoard//////////////////////////////////////////////
Keyboard::Keyboard(){
	keystates.reset();
	prevKeyStates.reset();
}
auto Keyboard::IsKeyDown(Key key) const noexcept -> bool
{
    return keystates.test(static_cast<uint16_t>(key));
}
auto Keyboard::IsKeyUp(Key key) const noexcept -> bool
{
    return !keystates.test(static_cast<uint16_t>(key));
}

auto Keyboard::IsKeyPressed(Key key) const noexcept -> bool
{
	const auto idx = static_cast<uint16_t>(key);
    return keystates.test(idx) && !prevKeyStates.test(idx);
}

auto Keyboard::IsKeyReleased(Key key) const noexcept -> bool
{
    const auto idx = static_cast<uint16_t>(key);
    return !keystates.test(idx) && prevKeyStates.test(idx);
}

auto Keyboard::ReadKey() noexcept -> std::optional<Keyboard::Event>
{
	if( !keybuffer.empty() )
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return {};
}


auto Keyboard::ReadChar() noexcept -> std::optional<unsigned char>
{
	if( !charbuffer.empty())
	{
		auto charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	return {};
}

auto Keyboard::FlushKey() noexcept -> void
{
	keybuffer = {};
}

auto Keyboard::FlushChar() noexcept -> void
{
	charbuffer = {};
}

auto Keyboard::Flush() noexcept -> void
{
	FlushKey();
	FlushChar();
}

auto Keyboard::EnableAutorepeat() noexcept -> void
{
	autorepeatEnabled = true;
}

auto Keyboard::DisableAutorepeat() noexcept -> void
{
	autorepeatEnabled = false;
}

auto Keyboard::AutorepeatIsEnabled() const noexcept -> bool
{
	return autorepeatEnabled;
}

auto Keyboard::UpdatePrevState() noexcept -> void
{
    prevKeyStates = keystates;
}

auto Keyboard::OnKeyPressed( Key key ) noexcept -> void
{
	const auto idx = static_cast<uint16_t>(key);
	keystates[idx] = true;
	keybuffer.emplace(Keyboard::Event::Type::Press, key);
	TrimBuffer(keybuffer);
}

auto Keyboard::OnKeyReleased( Key key ) noexcept -> void
{
	const auto idx = static_cast<uint16_t>(key);
	keystates[idx] = false;
	keybuffer.emplace(Keyboard::Event::Type::Release, key);
	TrimBuffer(keybuffer);
}

auto Keyboard::OnChar( unsigned char character ) noexcept -> void
{
	charbuffer.push( character );
	TrimBuffer( charbuffer );
}

auto Keyboard::ClearState() noexcept -> void
{
	keystates.reset();
}

template<typename Container>
requires requires(Container c){ c.size(); c.pop(); }
auto Keyboard::TrimBuffer( Container& buffer ) noexcept -> void
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}
/////////////////////////////////////////////////////////

namespace KeyMapper {

#if defined(WINDOWS_PLT)
uint32_t ToNative(Key key) {

	switch (key) {
		// Alphabet
		case Key::A:	return 'A';
		case Key::B:	return 'B';
		case Key::C:	return 'C';
		case Key::D:	return 'D';
		case Key::E:	return 'E';
		case Key::F:	return 'F';
		case Key::G:	return 'G';
		case Key::H:	return 'H';
		case Key::I:	return 'I';
		case Key::J:	return 'J';
		case Key::K:	return 'K';
		case Key::L:	return 'L';
		case Key::M:	return 'M';
		case Key::N:	return 'N';
		case Key::O:	return 'O';
		case Key::P:	return 'P';
		case Key::Q:	return 'Q';
		case Key::R:	return 'R';
		case Key::S:	return 'S';
		case Key::T:	return 'T';
		case Key::U:	return 'U';
		case Key::V:	return 'V';
		case Key::W:	return 'W';
		case Key::X:	return 'X';
		case Key::Y:	return 'Y';
		case Key::Z:	return 'Z';
		// Numbers
		case Key::Num0:	return '0';
		case Key::Num1:	return '1';
		case Key::Num2:	return '2';
		case Key::Num3:	return '3';
		case Key::Num4:	return '4';
		case Key::Num5:	return '5';
		case Key::Num6:	return '6';
		case Key::Num7:	return '7';
		case Key::Num8:	return '8';
		case Key::Num9:	return '9';
		// FXX
		case Key::F1:	return VK_F1;
        case Key::F2:	return VK_F2;
        case Key::F3:	return VK_F3;
        case Key::F4:	return VK_F4;
        case Key::F5:	return VK_F5;
        case Key::F6:	return VK_F6;
        case Key::F7:	return VK_F7;
        case Key::F8:	return VK_F8;
        case Key::F9:	return VK_F9;
        case Key::F10:	return VK_F10;
        case Key::F11:	return VK_F11;
        case Key::F12:	return VK_F12;
        case Key::F13:	return VK_F13;
        case Key::F14:	return VK_F14;
        case Key::F15:	return VK_F15;
        case Key::F16:	return VK_F16;
        case Key::F17:	return VK_F17;
        case Key::F18:	return VK_F18;
        case Key::F19:	return VK_F19;
        case Key::F20:	return VK_F20;
        case Key::F21:	return VK_F21;
        case Key::F22:	return VK_F22;
        case Key::F23:	return VK_F23;
        case Key::F24:	return VK_F24;
		// Specials
		case Key::Escape:	return VK_ESCAPE;
        case Key::Enter:	return VK_RETURN;
        case Key::Tab:	return VK_TAB;
        case Key::Backspace:	return VK_BACK;
        case Key::Space:	return VK_SPACE;
        case Key::CapsLock:	return VK_CAPITAL;
        case Key::PrintScreen:	return VK_SNAPSHOT;
        case Key::ScrollLock:	return VK_SCROLL;
        case Key::Pause:	return VK_PAUSE;
		// Navigations
		case Key::Insert:	return VK_INSERT;
        case Key::Home:	return VK_HOME;
        case Key::PageUp:	return VK_PRIOR;
        case Key::Delete:	return VK_DELETE;
        case Key::End:	return VK_END;
        case Key::PageDown:	return VK_NEXT;
		// Arrow
		case Key::Left:	return VK_LEFT;
		case Key::Right:	return VK_RIGHT;
		case Key::Up:	return VK_UP;
		case Key::Down:	return VK_DOWN;
		// Modifiers
		case Key::LeftShift:	return VK_LSHIFT;
		case Key::LeftControl:	return VK_LCONTROL;
		case Key::LeftAlt:		return VK_LMENU;
		case Key::LeftSuper:	return VK_LWIN;
		case Key::RightShift:	return VK_RSHIFT;
		case Key::RightControl: return VK_RCONTROL;
		case Key::RightAlt: 	return VK_RMENU;
		case Key::RightSuper:	return VK_RWIN;
		// Numpad
        case Key::NumPad0:	return VK_NUMPAD0;
        case Key::NumPad1:	return VK_NUMPAD1;
        case Key::NumPad2:	return VK_NUMPAD2;
        case Key::NumPad3:	return VK_NUMPAD3;
        case Key::NumPad4:	return VK_NUMPAD4;
        case Key::NumPad5:	return VK_NUMPAD5;
        case Key::NumPad6:	return VK_NUMPAD6;
        case Key::NumPad7:	return VK_NUMPAD7;
        case Key::NumPad8:	return VK_NUMPAD8;
        case Key::NumPad9:	return VK_NUMPAD9;
        case Key::NumPadDecimal:	return VK_DECIMAL;
        case Key::NumPadDivide:	return VK_DIVIDE;
        case Key::NumPadMultiply:	return VK_MULTIPLY;
        case Key::NumPadSubtract:	return VK_SUBTRACT;
        case Key::NumPadAdd:	return VK_ADD;
        case Key::NumPadEnter:	return VK_RETURN;
        case Key::NumPadEqual:	return VK_SEPARATOR;
		// Punctuation
        case Key::Comma:	return VK_OEM_COMMA;
        case Key::Period:	return VK_OEM_PERIOD;
        case Key::Semicolon:	return VK_OEM_1;
        case Key::Apostrophe:	return VK_OEM_7;
        case Key::BracketLeft:	return VK_OEM_4;
        case Key::BracketRight:	return VK_OEM_6;
        case Key::Backslash:	return VK_OEM_5;
        case Key::Slash:	return VK_OEM_2;
        case Key::GraveAccent:	return VK_OEM_3;
        case Key::Minus:	return VK_OEM_MINUS;
        case Key::Equal:	return VK_OEM_PLUS;

        default: 	std::unreachable();
    }
	return uint32_t{};
}

#elif defined(LINUX_PLT)

KeySym ToNative(Key key) {
    switch (key) {
		// Alphabet
		case Key::A:	return XK_A;
		case Key::B:	return XK_B;
		case Key::C:	return XK_C;
		case Key::D:	return XK_D;
		case Key::E:	return XK_E;
		case Key::F:	return XK_F;
		case Key::G:	return XK_G;
		case Key::H:	return XK_H;
		case Key::I:	return XK_I;
		case Key::J:	return XK_J;
		case Key::K:	return XK_K;
		case Key::L:	return XK_L;
		case Key::M:	return XK_M;
		case Key::N:	return XK_N;
		case Key::O:	return XK_O;
		case Key::P:	return XK_P;
		case Key::Q:	return XK_Q;
		case Key::R:	return XK_R;
		case Key::S:	return XK_S;
		case Key::T:	return XK_T;
		case Key::U:	return XK_U;
		case Key::V:	return XK_V;
		case Key::W:	return XK_W;
		case Key::X:	return XK_X;
		case Key::Y:	return XK_Y;
		case Key::Z:	return XK_Z;
		// Numbers
		case Key::Num0:	return XK_0;
		case Key::Num1:	return XK_1;
		case Key::Num2:	return XK_2;
		case Key::Num3:	return XK_3;
		case Key::Num4:	return XK_4;
		case Key::Num5:	return XK_5;
		case Key::Num6:	return XK_6;
		case Key::Num7:	return XK_7;
		case Key::Num8:	return XK_8;
		case Key::Num9:	return XK_9;
		// FXX
		case Key::F1:	return XK_F1;
        case Key::F2:	return XK_F2;
        case Key::F3:	return XK_F3;
        case Key::F4:	return XK_F4;
        case Key::F5:	return XK_F5;
        case Key::F6:	return XK_F6;
        case Key::F7:	return XK_F7;
        case Key::F8:	return XK_F8;
        case Key::F9:	return XK_F9;
        case Key::F10:	return XK_F10;
        case Key::F11:	return XK_F11;
        case Key::F12:	return XK_F12;
        case Key::F13:	return XK_F13;
        case Key::F14:	return XK_F14;
        case Key::F15:	return XK_F15;
        case Key::F16:	return XK_F16;
        case Key::F17:	return XK_F17;
        case Key::F18:	return XK_F18;
        case Key::F19:	return XK_F19;
        case Key::F20:	return XK_F20;
        case Key::F21:	return XK_F21;
        case Key::F22:	return XK_F22;
        case Key::F23:	return XK_F23;
        case Key::F24:	return XK_F24;
		//  Specials
        case Key::Escape:		return XK_Escape;
        case Key::Enter:			return XK_Return;
        case Key::Tab:			return XK_Tab;
        case Key::Backspace:		return XK_BackSpace;
        case Key::Space:			return XK_space;
        case Key::CapsLock:		return XK_Caps_Lock;
        case Key::PrintScreen:	return XK_Print;
        case Key::ScrollLock:	return XK_Scroll_Lock;
        case Key::Pause:			return XK_Pause;
		// Navigations
        case Key::Insert:	return XK_Insert;
        case Key::Home:		return XK_Home;
        case Key::PageUp:	return XK_Prior;
        case Key::Delete:	return XK_Delete;
        case Key::End:		return XK_End;
        case Key::PageDown:	return XK_Next;
		// Arrows
        case Key::Left:		return XK_Left;
        case Key::Right:		return XK_Right;
        case Key::Up:		return XK_Up;
        case Key::Down:		return XK_Down;
		// Modifiers
		case Key::LeftShift:		return XK_Shift_L;
        case Key::LeftControl:	return XK_Control_L;
        case Key::LeftAlt:		return XK_Alt_L;
        case Key::LeftSuper:		return XK_Super_L;
        case Key::RightShift:	return XK_Shift_R;
        case Key::RightControl:	return XK_Control_R;
        case Key::RightAlt:		return XK_Alt_R;
        case Key::RightSuper:	return XK_Super_R;
		// Numpad

        case Key::NumPad0:		return XK_KP_0;
        case Key::NumPad1:		return XK_KP_1;
        case Key::NumPad2:		return XK_KP_2;
        case Key::NumPad3:		return XK_KP_3;
        case Key::NumPad4:		return XK_KP_4;
        case Key::NumPad5:		return XK_KP_5;
        case Key::NumPad6:		return XK_KP_6;
        case Key::NumPad7:		return XK_KP_7;
        case Key::NumPad8:		return XK_KP_8;
        case Key::NumPad9:		return XK_KP_9;
        case Key::NumPadDecimal:	return XK_KP_Decimal;
        case Key::NumPadDivide:	return XK_KP_Divide;
        case Key::NumPadMultiply:return XK_KP_Multiply;
        case Key::NumPadSubtract:return XK_KP_Subtract;
        case Key::NumPadAdd:		return XK_KP_Add;
        case Key::NumPadEnter:	return XK_KP_Enter;
        case Key::NumPadEqual:	return XK_KP_Equal;
		// Punctuation
        case Key::Comma:			return XK_comma;
        case Key::Period:		return XK_period;
        case Key::Semicolon:		return XK_semicolon;
        case Key::Apostrophe:	return XK_apostrophe;
        case Key::BracketLeft:	return XK_bracketleft;
        case Key::BracketRight:	return XK_bracketright;
        case Key::Backslash:		return XK_backslash;
        case Key::Slash:			return XK_slash;
        case Key::GraveAccent:	return XK_grave;
        case Key::Minus:			return XK_minus;
        case Key::Equal:			return XK_equal;

        default:	std::unreachable();
	}
	return KeySym{};
}

#elif defined(WEB_PLT)
const char* ToNative(Key key) {
    switch (key) {
        // Alphanumeric keys
        case Key::A: 	return "KeyA";
        case Key::B: 	return "KeyB";
        case Key::C:	return "KeyC";
		case Key::D:	return "KeyD";
		case Key::E:	return "KeyE";
		case Key::F:	return "KeyF";
		case Key::G:	return "KeyG";
		case Key::H:	return "KeyH";
		case Key::I:	return "KeyI";
		case Key::J:	return "KeyJ";
		case Key::K:	return "KeyK";
		case Key::L:	return "KeyL";
		case Key::M:	return "KeyM";
		case Key::N:	return "KeyN";
		case Key::O:	return "KeyO";
		case Key::P:	return "KeyP";
		case Key::Q:	return "KeyQ";
		case Key::R:	return "KeyR";
		case Key::S:	return "KeyS";
		case Key::T:	return "KeyT";
		case Key::U:	return "KeyU";
		case Key::V:	return "KeyV";
		case Key::W:	return "KeyW";
		case Key::X:	return "KeyX";
		case Key::Y:	return "KeyY";
		case Key::Z:	return "KeyZ";
		// Numbers
		case Key::Num0:	return "Digit0";
		case Key::Num1:	return "Digit1";
		case Key::Num2:	return "Digit2";
		case Key::Num3:	return "Digit3";
		case Key::Num4:	return "Digit4";
		case Key::Num5:	return "Digit5";
		case Key::Num6:	return "Digit6";
		case Key::Num7:	return "Digit7";
		case Key::Num8:	return "Digit8";
		case Key::Num9:	return "Digit9";
        // FXX
        case Key::F1:	return "F1";
        case Key::F2:	return "F2";
        case Key::F3:	return "F3";
        case Key::F4:	return "F4";
        case Key::F5:	return "F5";
        case Key::F6:	return "F6";
        case Key::F7:	return "F7";
        case Key::F8:	return "F8";
        case Key::F9:	return "F9";
        case Key::F10:	return "F10";
        case Key::F11:	return "F11";
        case Key::F12:	return "F12";
        case Key::F13:	return "F13";
        case Key::F14:	return "F14";
        case Key::F15:	return "F15";
        case Key::F16:	return "F16";
        case Key::F17:	return "F17";
        case Key::F18:	return "F18";
        case Key::F19:	return "F19";
        case Key::F20:	return "F20";
        case Key::F21:	return "F21";
        case Key::F22:	return "F22";
        case Key::F23:	return "F23";
        case Key::F24:	return "F24";
        // Specials
        case Key::Escape: 		return "Escape";
        case Key::Enter: 		return "Enter";
        case Key::Tab: 			return "Tab";
        case Key::Backspace:	return "Backspace";
        case Key::Space: 		return "Space";
        case Key::CapsLock: 	return "CapsLock";
        case Key::PrintScreen:	return "PrintScreen";
        case Key::ScrollLock: 	return "ScrollLock";
        case Key::Pause: 		return "Pause";
        // Navigations
        case Key::Insert: 	return "Insert";
        case Key::Home: 	return "Home";
        case Key::PageUp: 	return "PageUp";
        case Key::Delete: 	return "Delete";
        case Key::End: 		return "End";
        case Key::PageDown: return "PageDown";
        // Arrows
        case Key::Left: 	return "ArrowLeft";
        case Key::Right: 	return "ArrowRight";
        case Key::Up: 		return "ArrowUp";
        case Key::Down: 	return "ArrowDown";
        // Modifiers
        case Key::LeftShift: return "ShiftLeft";
        case Key::LeftControl: return "ControlLeft";
        case Key::LeftAlt: return "AltLeft";
        case Key::LeftSuper: return "MetaLeft";
        case Key::RightShift: return "ShiftRight";
        case Key::RightControl: return "ControlRight";
        case Key::RightAlt: return "AltRight";
        case Key::RightSuper: return "MetaRight";
        // Numpad
        case Key::NumPad0: return "Numpad0";
        case Key::NumPad1: return "Numpad1";
        case Key::NumPad2: return "Numpad2";
        case Key::NumPad3: return "Numpad3";
        case Key::NumPad4: return "Numpad4";
        case Key::NumPad5: return "Numpad5";
        case Key::NumPad6: return "Numpad6";
        case Key::NumPad7: return "Numpad7";
        case Key::NumPad8: return "Numpad8";
        case Key::NumPad9: return "Numpad9";
		case Key::NumPadDecimal: 	return "NumpadDecimal";
        case Key::NumPadDivide: 	return "NumpadDivide";
        case Key::NumPadMultiply: 	return "NumpadMultiply";
        case Key::NumPadSubtract: 	return "NumpadSubtract";
        case Key::NumPadAdd: 		return "NumpadAdd";
        case Key::NumPadEnter: 		return "NumpadEnter";
        case Key::NumPadEqual: 		return "NumpadEqual";
        // Punctuation
        case Key::Comma: 		return "Comma";
        case Key::Period: 		return "Period";
        case Key::Semicolon: 	return "Semicolon";
        case Key::Apostrophe: 	return "Quote";
        case Key::BracketLeft: 	return "BracketLeft";
        case Key::BracketRight: return "BracketRight";
        case Key::Backslash: 	return "Backslash";
        case Key::Slash: 		return "Slash";
        case Key::GraveAccent: 	return "Backquote";
        case Key::Minus: 		return "Minus";
        case Key::Equal: 		return "Equal";
        // Web-specific
        case Key::BrowserBack: 		return "BrowserBack";
        case Key::BrowserForward: 	return "BrowserForward";
        case Key::BrowserRefresh: 	return "BrowserRefresh";
        case Key::BrowserStop: 		return "BrowserStop";
        case Key::BrowserSearch: 	return "BrowserSearch";
        case Key::BrowserFavorites: return "BrowserFavorites";
        case Key::BrowserHome: 		return "BrowserHome";
        case Key::VolumeMute: 		return "VolumeMute";
        case Key::VolumeDown: 		return "VolumeDown";
        case Key::VolumeUp: 		return "VolumeUp";
        case Key::MediaNext: 		return "MediaTrackNext";
        case Key::MediaPrevious: 	return "MediaTrackPrevious";
        case Key::MediaStop: 		return "MediaStop";
        case Key::MediaPlayPause: 	return "MediaPlayPause";

        default: std::unreachable(); 
    }
	return const char*{};
}
#endif
} // namespace KeyMapper