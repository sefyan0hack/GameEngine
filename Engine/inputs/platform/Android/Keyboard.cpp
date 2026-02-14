#include "Keyboard.hpp"
#include <core/Log.hpp>
#include <android/keycodes.h>
#include <engine_export.h>

ENGINE_EXPORT auto from_native(int32_t key) -> Key
{
    switch (key) {
        // Alphabet
        case AKEYCODE_A: return Key::A;
        case AKEYCODE_B: return Key::B;
        case AKEYCODE_C: return Key::C;
        case AKEYCODE_D: return Key::D;
        case AKEYCODE_E: return Key::E;
        case AKEYCODE_F: return Key::F;
        case AKEYCODE_G: return Key::G;
        case AKEYCODE_H: return Key::H;
        case AKEYCODE_I: return Key::I;
        case AKEYCODE_J: return Key::J;
        case AKEYCODE_K: return Key::K;
        case AKEYCODE_L: return Key::L;
        case AKEYCODE_M: return Key::M;
        case AKEYCODE_N: return Key::N;
        case AKEYCODE_O: return Key::O;
        case AKEYCODE_P: return Key::P;
        case AKEYCODE_Q: return Key::Q;
        case AKEYCODE_R: return Key::R;
        case AKEYCODE_S: return Key::S;
        case AKEYCODE_T: return Key::T;
        case AKEYCODE_U: return Key::U;
        case AKEYCODE_V: return Key::V;
        case AKEYCODE_W: return Key::W;
        case AKEYCODE_X: return Key::X;
        case AKEYCODE_Y: return Key::Y;
        case AKEYCODE_Z: return Key::Z;

        // Numbers
        case AKEYCODE_0: return Key::Num0;
        case AKEYCODE_1: return Key::Num1;
        case AKEYCODE_2: return Key::Num2;
        case AKEYCODE_3: return Key::Num3;
        case AKEYCODE_4: return Key::Num4;
        case AKEYCODE_5: return Key::Num5;
        case AKEYCODE_6: return Key::Num6;
        case AKEYCODE_7: return Key::Num7;
        case AKEYCODE_8: return Key::Num8;
        case AKEYCODE_9: return Key::Num9;

        // FXX (Note: Not all F-keys are supported on all Android devices/keyboards)
        case AKEYCODE_F1:   return Key::F1;
        case AKEYCODE_F2:   return Key::F2;
        case AKEYCODE_F3:   return Key::F3;
        case AKEYCODE_F4:   return Key::F4;
        case AKEYCODE_F5:   return Key::F5;
        case AKEYCODE_F6:   return Key::F6;
        case AKEYCODE_F7:   return Key::F7;
        case AKEYCODE_F8:   return Key::F8;
        case AKEYCODE_F9:   return Key::F9;
        case AKEYCODE_F10:  return Key::F10;
        case AKEYCODE_F11:  return Key::F11;
        case AKEYCODE_F12:  return Key::F12;

        // Specials
        case AKEYCODE_ESCAPE:       return Key::Escape;
        case AKEYCODE_ENTER:        return Key::Enter;
        case AKEYCODE_TAB:          return Key::Tab;
        case AKEYCODE_DEL:    return Key::Backspace;
        case AKEYCODE_SPACE:        return Key::Space;
        case AKEYCODE_CAPS_LOCK:     return Key::CapsLock;
        case AKEYCODE_SYSRQ:  return Key::PrintScreen;
        case AKEYCODE_SCROLL_LOCK:   return Key::ScrollLock;
        case AKEYCODE_BREAK:        return Key::Pause;

        // Navigations
        case AKEYCODE_INSERT:   return Key::Insert;
        case AKEYCODE_MOVE_HOME:     return Key::Home;
        case AKEYCODE_PAGE_UP:   return Key::PageUp;
        case AKEYCODE_FORWARD_DEL:   return Key::Delete;
        case AKEYCODE_MOVE_END:      return Key::End;
        case AKEYCODE_PAGE_DOWN: return Key::PageDown;

        // Arrows
        case AKEYCODE_DPAD_LEFT:     return Key::Left;
        case AKEYCODE_DPAD_RIGHT:    return Key::Right;
        case AKEYCODE_DPAD_UP:       return Key::Up;
        case AKEYCODE_DPAD_DOWN:     return Key::Down;

        // Modifiers
        case AKEYCODE_SHIFT_LEFT:    return Key::LeftShift;
        case AKEYCODE_CTRL_LEFT:  return Key::LeftControl;
        case AKEYCODE_ALT_LEFT:      return Key::LeftAlt;
        case AKEYCODE_META_LEFT:    return Key::LeftSuper;
        case AKEYCODE_SHIFT_RIGHT:   return Key::RightShift;
        case AKEYCODE_CTRL_RIGHT: return Key::RightControl;
        case AKEYCODE_ALT_RIGHT:     return Key::RightAlt;
        case AKEYCODE_META_RIGHT:   return Key::RightSuper;

        // Numpad
        case AKEYCODE_NUMPAD_0:      return Key::NumPad0;
        case AKEYCODE_NUMPAD_1:      return Key::NumPad1;
        case AKEYCODE_NUMPAD_2:      return Key::NumPad2;
        case AKEYCODE_NUMPAD_3:      return Key::NumPad3;
        case AKEYCODE_NUMPAD_4:      return Key::NumPad4;
        case AKEYCODE_NUMPAD_5:      return Key::NumPad5;
        case AKEYCODE_NUMPAD_6:      return Key::NumPad6;
        case AKEYCODE_NUMPAD_7:      return Key::NumPad7;
        case AKEYCODE_NUMPAD_8:      return Key::NumPad8;
        case AKEYCODE_NUMPAD_9:      return Key::NumPad9;
        case AKEYCODE_NUMPAD_ENTER:  return Key::NumPadEnter;
        case AKEYCODE_NUMPAD_ADD:    return Key::NumPadAdd;
        case AKEYCODE_NUMPAD_DIVIDE:      return Key::NumPadDivide;
        case AKEYCODE_NUMPAD_MULTIPLY:      return Key::NumPadMultiply;
        case AKEYCODE_NUMPAD_SUBTRACT:      return Key::NumPadSubtract;
        case AKEYCODE_NUMPAD_DOT:      return Key::NumPadDecimal;
        // case AKEYCODE_NUMPAD_COMMA:      return Key::Numxxx;
        case AKEYCODE_NUMPAD_EQUALS:      return Key::NumPadEqual;

        // Punctuation
        case AKEYCODE_COMMA:        return Key::Comma;
        case AKEYCODE_PERIOD:       return Key::Period;
        case AKEYCODE_SEMICOLON:    return Key::Semicolon;
        case AKEYCODE_APOSTROPHE:   return Key::Apostrophe;
        case AKEYCODE_LEFT_BRACKET:  return Key::BracketLeft;
        case AKEYCODE_RIGHT_BRACKET: return Key::BracketRight;
        case AKEYCODE_BACKSLASH:    return Key::Backslash;
        case AKEYCODE_SLASH:        return Key::Slash;
        case AKEYCODE_GRAVE:        return Key::GraveAccent;
        case AKEYCODE_MINUS:        return Key::Minus;
        case AKEYCODE_EQUALS:        return Key::Equal;

        case AKEYCODE_BACK:         return Key::Back;
        case AKEYCODE_MENU:         return Key::Menu;
        case AKEYCODE_SEARCH:       return Key::Search;

        case AKEYCODE_VOLUME_MUTE:    return Key::VolumeMute;
        case AKEYCODE_VOLUME_DOWN:    return Key::VolumeDown;
        case AKEYCODE_VOLUME_UP:      return Key::VolumeUp;
        case AKEYCODE_MEDIA_NEXT:     return Key::MediaNext;
        case AKEYCODE_MEDIA_PREVIOUS: return Key::MediaPrevious;
        case AKEYCODE_MEDIA_STOP:     return Key::MediaStop;
        case AKEYCODE_MEDIA_PLAY_PAUSE: return  Key::MediaPlayPause;
        default:  return Key::Unknown;
    }
}