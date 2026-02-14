#include "Keyboard.hpp"
#include <windows.h>
#include <engine_export.h>

ENGINE_EXPORT auto from_native(WPARAM key, LPARAM Lpr) -> Key
{
    const bool extended = (Lpr & (1u << 24)) != 0;
	const UINT scancode = (Lpr & 0x00FF0000) >> 16;

    switch (key) {
        // Alphabet
        case 'A': return Key::A;
        case 'B': return Key::B;
        case 'C': return Key::C;
        case 'D': return Key::D;
        case 'E': return Key::E;
        case 'F': return Key::F;
        case 'G': return Key::G;
        case 'H': return Key::H;
        case 'I': return Key::I;
        case 'J': return Key::J;
        case 'K': return Key::K;
        case 'L': return Key::L;
        case 'M': return Key::M;
        case 'N': return Key::N;
        case 'O': return Key::O;
        case 'P': return Key::P;
        case 'Q': return Key::Q;
        case 'R': return Key::R;
        case 'S': return Key::S;
        case 'T': return Key::T;
        case 'U': return Key::U;
        case 'V': return Key::V;
        case 'W': return Key::W;
        case 'X': return Key::X;
        case 'Y': return Key::Y;
        case 'Z': return Key::Z;
        // Numbers
        case '0': return Key::Num0;
        case '1': return Key::Num1;
        case '2': return Key::Num2;
        case '3': return Key::Num3;
        case '4': return Key::Num4;
        case '5': return Key::Num5;
        case '6': return Key::Num6;
        case '7': return Key::Num7;
        case '8': return Key::Num8;
        case '9': return Key::Num9;
        // FXX
        case VK_F1:	return Key::F1;
        case VK_F2:	return Key::F2;
        case VK_F3:	return Key::F3;
        case VK_F4:	return Key::F4;
        case VK_F5:	return Key::F5;
        case VK_F6:	return Key::F6;
        case VK_F7:	return Key::F7;
        case VK_F8:	return Key::F8;
        case VK_F9:	return Key::F9;
        case VK_F10: return Key::F10;
        case VK_F11: return Key::F11;
        case VK_F12: return Key::F12;
        case VK_F13: return Key::F13;
        case VK_F14: return Key::F14;
        case VK_F15: return Key::F15;
        case VK_F16: return Key::F16;
        case VK_F17: return Key::F17;
        case VK_F18: return Key::F18;
        case VK_F19: return Key::F19;
        case VK_F20: return Key::F20;
        case VK_F21: return Key::F21;
        case VK_F22: return Key::F22;
        case VK_F23: return Key::F23;
        case VK_F24: return Key::F24;
        // Specials
        case VK_ESCAPE:	    return  Key::Escape;
        case VK_RETURN:	    return  extended ? Key::NumPadEnter : Key::Enter;
        case VK_TAB:	    return  Key::Tab;
        case VK_BACK:	    return  Key::Backspace;
        case VK_SPACE:	    return  Key::Space;
        case VK_CAPITAL:    return  Key::CapsLock;
        case VK_SNAPSHOT:   return  Key::PrintScreen;
        case VK_SCROLL:     return  Key::ScrollLock;
        case VK_PAUSE:	    return  Key::Pause;
        // Navigations
        case VK_INSERT:	return Key::Insert;
        case VK_HOME:	return Key::Home;
        case VK_PRIOR:	return Key::PageUp;
        case VK_DELETE:	return Key::Delete;
        case VK_END:	return Key::End;
        case VK_NEXT:	return Key::PageDown;
        // Arrow
        case VK_LEFT:	    return Key::Left;
        case VK_RIGHT:	return Key::Right;
        case VK_UP:	    return Key::Up;
        case VK_DOWN:	    return Key::Down;

        // Modifiers
        
        case VK_SHIFT: return (MapVirtualKeyA(scancode, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT) ? Key::RightShift : Key::LeftShift;
		case VK_CONTROL: return extended ? Key::RightControl : Key::LeftControl;
		case VK_MENU: return extended ? Key::RightAlt : Key::LeftAlt;

        case VK_LSHIFT:	return Key::LeftShift;
        case VK_LCONTROL:	return Key::LeftControl;
        case VK_LMENU:		return Key::LeftAlt;
        case VK_LWIN:	return Key::LeftSuper;
        case VK_RSHIFT:	return Key::RightShift;
        case VK_RCONTROL: return Key::RightControl;
        case VK_RMENU: 	return Key::RightAlt;
        case VK_RWIN:	return Key::RightSuper;

        // Numpad
        case VK_NUMPAD0:	return Key::NumPad0;
        case VK_NUMPAD1:	return Key::NumPad1;
        case VK_NUMPAD2:	return Key::NumPad2;
        case VK_NUMPAD3:	return Key::NumPad3;
        case VK_NUMPAD4:	return Key::NumPad4;
        case VK_NUMPAD5:	return Key::NumPad5;
        case VK_NUMPAD6:	return Key::NumPad6;
        case VK_NUMPAD7:	return Key::NumPad7;
        case VK_NUMPAD8:	return Key::NumPad8;
        case VK_NUMPAD9:	return Key::NumPad9;
        case VK_DECIMAL:	return Key::NumPadDecimal;
        case VK_DIVIDE:	    return Key::NumPadDivide;
        case VK_MULTIPLY:	return Key::NumPadMultiply;
        case VK_SUBTRACT:	return Key::NumPadSubtract;
        case VK_ADD:	    return Key::NumPadAdd;
        case VK_SEPARATOR:	    return Key::NumPadEqual;
        // Punctuation
        case VK_OEM_COMMA:	    return Key::Comma;
        case VK_OEM_PERIOD:	    return Key::Period;
        case VK_OEM_1:	return Key::Semicolon;
        case VK_OEM_7:	return Key::Apostrophe;
        case VK_OEM_4:	return Key::BracketLeft;
        case VK_OEM_6:	return Key::BracketRight;
        case VK_OEM_5:	return Key::Backslash;
        case VK_OEM_2:	    return Key::Slash;
        case VK_OEM_3:	return Key::GraveAccent;
        case VK_OEM_MINUS:	    return Key::Minus;
        case VK_OEM_PLUS:	    return Key::Equal;
        default: return Key::Unknown;
    }
}