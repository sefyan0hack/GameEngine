#include "Keyboard.hpp"
#include <core/Log.hpp>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <engine_export.h>

ENGINE_EXPORT auto from_native(KeySym key) -> Key
{
    switch (key) {
        // Alphabet [A-Z]
        case XK_A:	return Key::A;
        case XK_B:	return Key::B;
        case XK_C:	return Key::C;
        case XK_D:	return Key::D;
        case XK_E:	return Key::E;
        case XK_F:	return Key::F;
        case XK_G:	return Key::G;
        case XK_H:	return Key::H;
        case XK_I:	return Key::I;
        case XK_J:	return Key::J;
        case XK_K:	return Key::K;
        case XK_L:	return Key::L;
        case XK_M:	return Key::M;
        case XK_N:	return Key::N;
        case XK_O:	return Key::O;
        case XK_P:	return Key::P;
        case XK_Q:	return Key::Q;
        case XK_R:	return Key::R;
        case XK_S:	return Key::S;
        case XK_T:	return Key::T;
        case XK_U:	return Key::U;
        case XK_V:	return Key::V;
        case XK_W:	return Key::W;
        case XK_X:	return Key::X;
        case XK_Y:	return Key::Y;
        case XK_Z:	return Key::Z;
        // Alphabet [a-z]
        case XK_a:	return Key::A;
        case XK_b:	return Key::B;
        case XK_c:	return Key::C;
        case XK_d:	return Key::D;
        case XK_e:	return Key::E;
        case XK_f:	return Key::F;
        case XK_g:	return Key::G;
        case XK_h:	return Key::H;
        case XK_i:	return Key::I;
        case XK_j:	return Key::J;
        case XK_k:	return Key::K;
        case XK_l:	return Key::L;
        case XK_m:	return Key::M;
        case XK_n:	return Key::N;
        case XK_o:	return Key::O;
        case XK_p:	return Key::P;
        case XK_q:	return Key::Q;
        case XK_r:	return Key::R;
        case XK_s:	return Key::S;
        case XK_t:	return Key::T;
        case XK_u:	return Key::U;
        case XK_v:	return Key::V;
        case XK_w:	return Key::W;
        case XK_x:	return Key::X;
        case XK_y:	return Key::Y;
        case XK_z:	return Key::Z;
        // Numbers
        case XK_0:	return Key::Num0;
        case XK_1:	return Key::Num1;
        case XK_2:	return Key::Num2;
        case XK_3:	return Key::Num3;
        case XK_4:	return Key::Num4;
        case XK_5:	return Key::Num5;
        case XK_6:	return Key::Num6;
        case XK_7:	return Key::Num7;
        case XK_8:	return Key::Num8;
        case XK_9:	return Key::Num9;
        // FXX
        case XK_F1:	return Key::F1;
        case XK_F2:	return Key::F2;
        case XK_F3:	return Key::F3;
        case XK_F4:	return Key::F4;
        case XK_F5:	return Key::F5;
        case XK_F6:	return Key::F6;
        case XK_F7:	return Key::F7;
        case XK_F8:	return Key::F8;
        case XK_F9:	return Key::F9;
        case XK_F10:	return Key::F10;
        case XK_F11:	return Key::F11;
        case XK_F12:	return Key::F12;
        case XK_F13:	return Key::F13;
        case XK_F14:	return Key::F14;
        case XK_F15:	return Key::F15;
        case XK_F16:	return Key::F16;
        case XK_F17:	return Key::F17;
        case XK_F18:	return Key::F18;
        case XK_F19:	return Key::F19;
        case XK_F20:	return Key::F20;
        case XK_F21:	return Key::F21;
        case XK_F22:	return Key::F22;
        case XK_F23:	return Key::F23;
        case XK_F24:	return Key::F24;
        //  Specials
        case XK_Escape:		return Key::Escape;
        case XK_Return:		return Key::Enter;
        case XK_Tab:			return Key::Tab;
        case XK_BackSpace:	return Key::Backspace;
        case XK_space:		return Key::Space;
        case XK_Caps_Lock:		return Key::CapsLock;
        case XK_Print:	return Key::PrintScreen;
        case XK_Scroll_Lock:	return Key::ScrollLock;
        case XK_Pause:		return Key::Pause;
        // Navigations
        case XK_Insert:	return Key::Insert;
        case XK_Home:		return Key::Home;
        case XK_Prior:	return Key::PageUp;
        case XK_Delete:	return Key::Delete;
        case XK_End:		return Key::End;
        case XK_Next:	return Key::PageDown;
        // Arrows
        case XK_Left:		return Key::Left;
        case XK_Right:	return Key::Right;
        case XK_Up:		return Key::Up;
        case XK_Down:		return Key::Down;
        // Modifiers
        case XK_Shift_L:	return Key::LeftShift;
        case XK_Control_L:	return Key::LeftControl;
        case XK_Alt_L:		return Key::LeftAlt;
        case XK_Super_L:	return Key::LeftSuper;
        case XK_Shift_R:	return Key::RightShift;
        case XK_Control_R:	return Key::RightControl;
        case XK_Alt_R:		return Key::RightAlt;
        case XK_Super_R:	return Key::RightSuper;
        // Numpad
        case XK_KP_0:		return Key::NumPad0;
        case XK_KP_1:		return Key::NumPad1;
        case XK_KP_2:		return Key::NumPad2;
        case XK_KP_3:		return Key::NumPad3;
        case XK_KP_4:		return Key::NumPad4;
        case XK_KP_5:		return Key::NumPad5;
        case XK_KP_6:		return Key::NumPad6;
        case XK_KP_7:		return Key::NumPad7;
        case XK_KP_8:		return Key::NumPad8;
        case XK_KP_9:		return Key::NumPad9;
        case XK_KP_Decimal:	return Key::NumPadDecimal;
        case XK_KP_Divide:	    return Key::NumPadDivide;
        case XK_KP_Multiply:   return Key::NumPadMultiply;
        case XK_KP_Subtract:   return Key::NumPadSubtract;
        case XK_KP_Add:		return Key::NumPadAdd;
        case XK_KP_Enter:	    return Key::NumPadEnter;
        case XK_KP_Equal:	    return Key::NumPadEqual;
        // Punctuation
        case XK_comma:		return Key::Comma;
        case XK_period:		return Key::Period;
        case XK_semicolon:	return Key::Semicolon;
        case XK_apostrophe:	return Key::Apostrophe;
        case XK_bracketleft:	return Key::BracketLeft;
        case XK_bracketright:	return Key::BracketRight;
        case XK_backslash:	return Key::Backslash;
        case XK_slash:		return Key::Slash;
        case XK_grave:	return Key::GraveAccent;
        case XK_minus:		return Key::Minus;
        case XK_equal:		return Key::Equal;
        default: return Key::Unknown;
    }
}