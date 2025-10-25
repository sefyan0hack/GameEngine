
auto Keyboard::to_native(Key key) -> uint32_t
{
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
        case Key::Enter:		return XK_Return;
        case Key::Tab:			return XK_Tab;
        case Key::Backspace:	return XK_BackSpace;
        case Key::Space:		return XK_space;
        case Key::CapsLock:		return XK_Caps_Lock;
        case Key::PrintScreen:	return XK_Print;
        case Key::ScrollLock:	return XK_Scroll_Lock;
        case Key::Pause:		return XK_Pause;
        // Navigations
        case Key::Insert:	return XK_Insert;
        case Key::Home:		return XK_Home;
        case Key::PageUp:	return XK_Prior;
        case Key::Delete:	return XK_Delete;
        case Key::End:		return XK_End;
        case Key::PageDown:	return XK_Next;
        // Arrows
        case Key::Left:		return XK_Left;
        case Key::Right:	return XK_Right;
        case Key::Up:		return XK_Up;
        case Key::Down:		return XK_Down;
        // Modifiers
        case Key::LeftShift:	return XK_Shift_L;
        case Key::LeftControl:	return XK_Control_L;
        case Key::LeftAlt:		return XK_Alt_L;
        case Key::LeftSuper:	return XK_Super_L;
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
        case Key::NumPadDivide:	    return XK_KP_Divide;
        case Key::NumPadMultiply:   return XK_KP_Multiply;
        case Key::NumPadSubtract:   return XK_KP_Subtract;
        case Key::NumPadAdd:		return XK_KP_Add;
        case Key::NumPadEnter:	    return XK_KP_Enter;
        case Key::NumPadEqual:	    return XK_KP_Equal;
        // Punctuation
        case Key::Comma:		return XK_comma;
        case Key::Period:		return XK_period;
        case Key::Semicolon:	return XK_semicolon;
        case Key::Apostrophe:	return XK_apostrophe;
        case Key::BracketLeft:	return XK_bracketleft;
        case Key::BracketRight:	return XK_bracketright;
        case Key::Backslash:	return XK_backslash;
        case Key::Slash:		return XK_slash;
        case Key::GraveAccent:	return XK_grave;
        case Key::Minus:		return XK_minus;
        case Key::Equal:		return XK_equal;
        case Key::Unknown: return 0;
        default: return static_cast<uint32_t>(-1);
    }
}