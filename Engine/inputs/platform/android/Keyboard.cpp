#include "Keyboard.hpp"
#include <core/Log.hpp>
#include <android/keycodes.h>


auto Keyboard::to_native(Key key) -> uint32_t
{
    switch (key) {
        // Alphabet
        case Key::A:    return AKEYCODE_A;
        case Key::B:    return AKEYCODE_B;
        case Key::C:    return AKEYCODE_C;
        case Key::D:    return AKEYCODE_D;
        case Key::E:    return AKEYCODE_E;
        case Key::F:    return AKEYCODE_F;
        case Key::G:    return AKEYCODE_G;
        case Key::H:    return AKEYCODE_H;
        case Key::I:    return AKEYCODE_I;
        case Key::J:    return AKEYCODE_J;
        case Key::K:    return AKEYCODE_K;
        case Key::L:    return AKEYCODE_L;
        case Key::M:    return AKEYCODE_M;
        case Key::N:    return AKEYCODE_N;
        case Key::O:    return AKEYCODE_O;
        case Key::P:    return AKEYCODE_P;
        case Key::Q:    return AKEYCODE_Q;
        case Key::R:    return AKEYCODE_R;
        case Key::S:    return AKEYCODE_S;
        case Key::T:    return AKEYCODE_T;
        case Key::U:    return AKEYCODE_U;
        case Key::V:    return AKEYCODE_V;
        case Key::W:    return AKEYCODE_W;
        case Key::X:    return AKEYCODE_X;
        case Key::Y:    return AKEYCODE_Y;
        case Key::Z:    return AKEYCODE_Z;

        // Numbers
        case Key::Num0: return AKEYCODE_0;
        case Key::Num1: return AKEYCODE_1;
        case Key::Num2: return AKEYCODE_2;
        case Key::Num3: return AKEYCODE_3;
        case Key::Num4: return AKEYCODE_4;
        case Key::Num5: return AKEYCODE_5;
        case Key::Num6: return AKEYCODE_6;
        case Key::Num7: return AKEYCODE_7;
        case Key::Num8: return AKEYCODE_8;
        case Key::Num9: return AKEYCODE_9;

        // FXX (Note: Not all F-keys are supported on all Android devices/keyboards)
        case Key::F1:   return AKEYCODE_F1;
        case Key::F2:   return AKEYCODE_F2;
        case Key::F3:   return AKEYCODE_F3;
        case Key::F4:   return AKEYCODE_F4;
        case Key::F5:   return AKEYCODE_F5;
        case Key::F6:   return AKEYCODE_F6;
        case Key::F7:   return AKEYCODE_F7;
        case Key::F8:   return AKEYCODE_F8;
        case Key::F9:   return AKEYCODE_F9;
        case Key::F10:  return AKEYCODE_F10;
        case Key::F11:  return AKEYCODE_F11;
        case Key::F12:  return AKEYCODE_F12;

        // Specials
        case Key::Escape:       return AKEYCODE_ESCAPE;
        case Key::Enter:        return AKEYCODE_ENTER;
        case Key::Tab:          return AKEYCODE_TAB;
        case Key::Backspace:    return AKEYCODE_DEL;
        case Key::Space:        return AKEYCODE_SPACE;
        case Key::CapsLock:     return AKEYCODE_CAPS_LOCK;
        case Key::PrintScreen:  return AKEYCODE_SYSRQ;
        case Key::ScrollLock:   return AKEYCODE_SCROLL_LOCK;
        case Key::Pause:        return AKEYCODE_BREAK;

        // Navigations
        case Key::Insert:   return AKEYCODE_INSERT;
        case Key::Home:     return AKEYCODE_MOVE_HOME;
        case Key::PageUp:   return AKEYCODE_PAGE_UP;
        case Key::Delete:   return AKEYCODE_FORWARD_DEL;
        case Key::End:      return AKEYCODE_MOVE_END;
        case Key::PageDown: return AKEYCODE_PAGE_DOWN;

        // Arrows
        case Key::Left:     return AKEYCODE_DPAD_LEFT;
        case Key::Right:    return AKEYCODE_DPAD_RIGHT;
        case Key::Up:       return AKEYCODE_DPAD_UP;
        case Key::Down:     return AKEYCODE_DPAD_DOWN;

        // Modifiers
        case Key::LeftShift:    return AKEYCODE_SHIFT_LEFT;
        case Key::LeftControl:  return AKEYCODE_CTRL_LEFT;
        case Key::LeftAlt:      return AKEYCODE_ALT_LEFT;
        case Key::LeftSuper:    return AKEYCODE_META_LEFT;
        case Key::RightShift:   return AKEYCODE_SHIFT_RIGHT;
        case Key::RightControl: return AKEYCODE_CTRL_RIGHT;
        case Key::RightAlt:     return AKEYCODE_ALT_RIGHT;
        case Key::RightSuper:   return AKEYCODE_META_RIGHT;

        // Numpad
        case Key::NumPad0:      return AKEYCODE_NUMPAD_0;
        case Key::NumPad1:      return AKEYCODE_NUMPAD_1;
        case Key::NumPad2:      return AKEYCODE_NUMPAD_2;
        case Key::NumPad3:      return AKEYCODE_NUMPAD_3;
        case Key::NumPad4:      return AKEYCODE_NUMPAD_4;
        case Key::NumPad5:      return AKEYCODE_NUMPAD_5;
        case Key::NumPad6:      return AKEYCODE_NUMPAD_6;
        case Key::NumPad7:      return AKEYCODE_NUMPAD_7;
        case Key::NumPad8:      return AKEYCODE_NUMPAD_8;
        case Key::NumPad9:      return AKEYCODE_NUMPAD_9;
        case Key::NumPadEnter:  return AKEYCODE_NUMPAD_ENTER;
        case Key::NumPadAdd:    return AKEYCODE_NUMPAD_ADD;
        case Key::NumPadDivide:      return AKEYCODE_NUMPAD_DIVIDE;
        case Key::NumPadMultiply:      return AKEYCODE_NUMPAD_MULTIPLY;
        case Key::NumPadSubtract:      return AKEYCODE_NUMPAD_SUBTRACT;
        case Key::NumPadDecimal:      return AKEYCODE_NUMPAD_DOT;
        // case Key::Num:      return AKEYCODE_NUMPAD_COMMA;
        case Key::NumPadEqual:      return AKEYCODE_NUMPAD_EQUALS;

        // Punctuation
        case Key::Comma:        return AKEYCODE_COMMA;
        case Key::Period:       return AKEYCODE_PERIOD;
        case Key::Semicolon:    return AKEYCODE_SEMICOLON;
        case Key::Apostrophe:   return AKEYCODE_APOSTROPHE;
        case Key::BracketLeft:  return AKEYCODE_LEFT_BRACKET;
        case Key::BracketRight: return AKEYCODE_RIGHT_BRACKET;
        case Key::Backslash:    return AKEYCODE_BACKSLASH;
        case Key::Slash:        return AKEYCODE_SLASH;
        case Key::GraveAccent:  return AKEYCODE_GRAVE;
        case Key::Minus:        return AKEYCODE_MINUS;
        case Key::Equal:        return AKEYCODE_EQUALS;

        case Key::Back:         return AKEYCODE_BACK;
        case Key::Menu:         return AKEYCODE_MENU;
        case Key::Search:       return AKEYCODE_SEARCH;

        case Key::VolumeMute:    return AKEYCODE_VOLUME_MUTE;
        case Key::VolumeDown:    return AKEYCODE_VOLUME_DOWN;
        case Key::VolumeUp:      return AKEYCODE_VOLUME_UP;
        case Key::MediaNext:     return AKEYCODE_MEDIA_NEXT;
        case Key::MediaPrevious: return AKEYCODE_MEDIA_PREVIOUS;
        case Key::MediaStop:     return AKEYCODE_MEDIA_STOP;
        case Key::MediaPlayPause:return AKEYCODE_MEDIA_PLAY_PAUSE;
        case Key::Unknown: return 0;
        default: return 0;
    }
}