#include "Keyboard.hpp"
#include <emscripten/key_codes.h>

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

auto Keyboard::to_native(Key key) -> uint32_t
{
    switch (key) {
        // Alphanumeric keys
        case Key::A: 	return DOM_VK_A;
        case Key::B: 	return DOM_VK_B;
        case Key::C:	return DOM_VK_C;
        case Key::D:	return DOM_VK_D;
        case Key::E:	return DOM_VK_E;
        case Key::F:	return DOM_VK_F;
        case Key::G:	return DOM_VK_G;
        case Key::H:	return DOM_VK_H;
        case Key::I:	return DOM_VK_I;
        case Key::J:	return DOM_VK_J;
        case Key::K:	return DOM_VK_K;
        case Key::L:	return DOM_VK_L;
        case Key::M:	return DOM_VK_M;
        case Key::N:	return DOM_VK_N;
        case Key::O:	return DOM_VK_O;
        case Key::P:	return DOM_VK_P;
        case Key::Q:	return DOM_VK_Q;
        case Key::R:	return DOM_VK_R;
        case Key::S:	return DOM_VK_S;
        case Key::T:	return DOM_VK_T;
        case Key::U:	return DOM_VK_U;
        case Key::V:	return DOM_VK_V;
        case Key::W:	return DOM_VK_W;
        case Key::X:	return DOM_VK_X;
        case Key::Y:	return DOM_VK_Y;
        case Key::Z:	return DOM_VK_Z;
        // Numbers
        case Key::Num0:	return DOM_VK_0;
        case Key::Num1:	return DOM_VK_1;
        case Key::Num2:	return DOM_VK_2;
        case Key::Num3:	return DOM_VK_3;
        case Key::Num4:	return DOM_VK_4;
        case Key::Num5:	return DOM_VK_5;
        case Key::Num6:	return DOM_VK_6;
        case Key::Num7:	return DOM_VK_7;
        case Key::Num8:	return DOM_VK_8;
        case Key::Num9:	return DOM_VK_9;
        // FXX keys
        case Key::F1:	return DOM_VK_F1;
        case Key::F2:	return DOM_VK_F2;
        case Key::F3:	return DOM_VK_F3;
        case Key::F4:	return DOM_VK_F4;
        case Key::F5:	return DOM_VK_F5;
        case Key::F6:	return DOM_VK_F6;
        case Key::F7:	return DOM_VK_F7;
        case Key::F8:	return DOM_VK_F8;
        case Key::F9:	return DOM_VK_F9;
        case Key::F10:	return DOM_VK_F10;
        case Key::F11:	return DOM_VK_F11;
        case Key::F12:	return DOM_VK_F12;
        case Key::F13:	return DOM_VK_F13;
        case Key::F14:	return DOM_VK_F14;
        case Key::F15:	return DOM_VK_F15;
        case Key::F16:	return DOM_VK_F16;
        case Key::F17:	return DOM_VK_F17;
        case Key::F18:	return DOM_VK_F18;
        case Key::F19:	return DOM_VK_F19;
        case Key::F20:	return DOM_VK_F20;
        case Key::F21:	return DOM_VK_F21;
        case Key::F22:	return DOM_VK_F22;
        case Key::F23:	return DOM_VK_F23;
        case Key::F24:	return DOM_VK_F24;
        // Special keys
        case Key::Escape: 	    return DOM_VK_ESCAPE;
        case Key::Enter: 	    return DOM_VK_RETURN;
        case Key::Tab: 		    return DOM_VK_TAB;
        case Key::Backspace:	return DOM_VK_BACK_SPACE;
        case Key::Space: 	    return DOM_VK_SPACE;
        case Key::CapsLock: 	return DOM_VK_CAPS_LOCK;
        case Key::PrintScreen:	return DOM_VK_PRINTSCREEN;
        case Key::ScrollLock: 	return DOM_VK_SCROLL_LOCK;
        case Key::Pause: 	    return DOM_VK_PAUSE;
        // Navigation keys
        case Key::Insert: 	return DOM_VK_INSERT;
        case Key::Home: 	return DOM_VK_HOME;
        case Key::PageUp: 	return DOM_VK_PAGE_UP;
        case Key::Delete: 	return DOM_VK_DELETE;
        case Key::End: 		return DOM_VK_END;
        case Key::PageDown: return DOM_VK_PAGE_DOWN;
        // Arrow keys
        case Key::Left: 	return DOM_VK_LEFT;
        case Key::Right: 	return DOM_VK_RIGHT;
        case Key::Up: 		return DOM_VK_UP;
        case Key::Down: 	return DOM_VK_DOWN;
        // Modifiers
        case Key::LeftShift: 
        case Key::RightShift: 
            return DOM_VK_SHIFT;
        case Key::LeftControl:
        case Key::RightControl:
            return DOM_VK_CONTROL;
        case Key::LeftAlt: return DOM_VK_ALT;
        case Key::RightAlt:return DOM_VK_ALTGR;
        case Key::LeftSuper:
        case Key::RightSuper:
            return DOM_VK_META;
        // Numpad keys
        case Key::NumPad0: 	return DOM_VK_NUMPAD0;
        case Key::NumPad1: 	return DOM_VK_NUMPAD1;
        case Key::NumPad2: 	return DOM_VK_NUMPAD2;
        case Key::NumPad3: 	return DOM_VK_NUMPAD3;
        case Key::NumPad4: 	return DOM_VK_NUMPAD4;
        case Key::NumPad5: 	return DOM_VK_NUMPAD5;
        case Key::NumPad6: 	return DOM_VK_NUMPAD6;
        case Key::NumPad7: 	return DOM_VK_NUMPAD7;
        case Key::NumPad8: 	return DOM_VK_NUMPAD8;
        case Key::NumPad9: 	return DOM_VK_NUMPAD9;
        case Key::NumPadDecimal: 	return DOM_VK_DECIMAL;
        case Key::NumPadDivide: 	return DOM_VK_DIVIDE;
        case Key::NumPadMultiply: 	return DOM_VK_MULTIPLY;
        case Key::NumPadSubtract: 	return DOM_VK_SUBTRACT;
        case Key::NumPadAdd: 	return DOM_VK_ADD;
        case Key::NumPadEnter: 	return DOM_VK_ENTER;
        case Key::NumPadEqual: 	return DOM_VK_EQUALS;
        // Punctuation keys
        case Key::Comma: 		return DOM_VK_COMMA;
        case Key::Period: 		return DOM_VK_PERIOD;
        case Key::Semicolon: 	return DOM_VK_SEMICOLON;
        case Key::Apostrophe: 	return DOM_VK_QUOTE;
        case Key::BracketLeft: 	return DOM_VK_OPEN_BRACKET;
        case Key::BracketRight: return DOM_VK_CLOSE_BRACKET;
        case Key::Backslash: 	return DOM_VK_BACK_SLASH;
        case Key::Slash: 		return DOM_VK_SLASH;
        case Key::GraveAccent: 	return DOM_VK_BACK_QUOTE;
        case Key::Minus: 		return DOM_VK_HYPHEN_MINUS;
        case Key::Equal: 		return DOM_VK_EQUALS;
       
        // Web-specific
        case Key::BrowserBack:         return DOM_VK_BROWSER_BACK;
        case Key::BrowserForward:      return DOM_VK_BROWSER_FORWARD;
        case Key::BrowserRefresh:      return DOM_VK_BROWSER_REFRESH;
        case Key::BrowserStop:         return DOM_VK_BROWSER_STOP;
        case Key::BrowserSearch:       return DOM_VK_BROWSER_SEARCH;
        case Key::BrowserFavorites:    return DOM_VK_BROWSER_FAVORITES;
        case Key::BrowserHome:         return DOM_VK_BROWSER_HOME;
        case Key::VolumeMute:          return DOM_VK_VOLUME_MUTE;
        case Key::VolumeDown:          return DOM_VK_VOLUME_DOWN;
        case Key::VolumeUp:            return DOM_VK_VOLUME_UP;
        case Key::MediaNext:           return DOM_VK_MEDIA_TRACK_NEXT;
        case Key::MediaPrevious:       return DOM_VK_MEDIA_TRACK_PREVIOUS;
        case Key::MediaStop:           return DOM_VK_MEDIA_STOP;
        case Key::MediaPlayPause:      return DOM_VK_MEDIA_PLAY_PAUSE;
        case Key::Unknown: return 0;
        default: return static_cast<uint32_t>(-1);
    }
}