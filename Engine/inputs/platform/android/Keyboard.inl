#include "Keyboard.hpp"
#include <core/Log.hpp>
#include <android/keycodes.h>

#define  KEY_NATIVE_MAP\
        X(A, AKEYCODE_A)\
        X(B, AKEYCODE_B)\
        X(C, AKEYCODE_C)\
        X(D, AKEYCODE_D)\
        X(E, AKEYCODE_E)\
        X(F, AKEYCODE_F)\
        X(G, AKEYCODE_G)\
        X(H, AKEYCODE_H)\
        X(I, AKEYCODE_I)\
        X(J, AKEYCODE_J)\
        X(K, AKEYCODE_K)\
        X(L, AKEYCODE_L)\
        X(M, AKEYCODE_M)\
        X(N, AKEYCODE_N)\
        X(O, AKEYCODE_O)\
        X(P, AKEYCODE_P)\
        X(Q, AKEYCODE_Q)\
        X(R, AKEYCODE_R)\
        X(S, AKEYCODE_S)\
        X(T, AKEYCODE_T)\
        X(U, AKEYCODE_U)\
        X(V, AKEYCODE_V)\
        X(W, AKEYCODE_W)\
        X(X, AKEYCODE_X)\
        X(Y, AKEYCODE_Y)\
        X(Z, AKEYCODE_Z)\
        \
        X(Num0, AKEYCODE_0)\
        X(Num1, AKEYCODE_1)\
        X(Num2, AKEYCODE_2)\
        X(Num3, AKEYCODE_3)\
        X(Num4, AKEYCODE_4)\
        X(Num5, AKEYCODE_5)\
        X(Num6, AKEYCODE_6)\
        X(Num7, AKEYCODE_7)\
        X(Num8, AKEYCODE_8)\
        X(Num9, AKEYCODE_9)\
        \
        X(F1,  AKEYCODE_F1)\
        X(F2,  AKEYCODE_F2)\
        X(F3,  AKEYCODE_F3)\
        X(F4,  AKEYCODE_F4)\
        X(F5,  AKEYCODE_F5)\
        X(F6,  AKEYCODE_F6)\
        X(F7,  AKEYCODE_F7)\
        X(F8,  AKEYCODE_F8)\
        X(F9,  AKEYCODE_F9)\
        X(F10, AKEYCODE_F10)\
        X(F11, AKEYCODE_F11)\
        X(F12, AKEYCODE_F12)\
        \
        (Escape, AKEYCODE_ESCAPE)\
        (Enter, AKEYCODE_ENTER)\
        (Tab, AKEYCODE_TAB)\
        (Backspace, AKEYCODE_DEL)\
        (Space, AKEYCODE_SPACE)\
        (CapsLock, AKEYCODE_CAPS_LOCK)\
        (PrintScreen, AKEYCODE_SYSRQ)\
        (ScrollLock, AKEYCODE_SCROLL_LOCK)\
        (Pause, AKEYCODE_BREAK)\
        \
        X(Insert,   AKEYCODE_INSERT)\
        X(Home, AKEYCODE_MOVE_HOME)\
        X(PageUp,   AKEYCODE_PAGE_UP)\
        X(Delete,   AKEYCODE_FORWARD_DEL)\
        X(End,  AKEYCODE_MOVE_END)\
        X(PageDown, AKEYCODE_PAGE_DOWN)\
        \
        X(Left, AKEYCODE_DPAD_LEFT)\
        X(Right, AKEYCODE_DPAD_RIGHT)\
        X(Up, AKEYCODE_DPAD_UP)\
        X(Down, AKEYCODE_DPAD_DOWN)\
        \
        X(LeftShift,    AKEYCODE_SHIFT_LEFT)\
        X(LeftControl,  AKEYCODE_CTRL_LEFT)\
        X(LeftAlt,  AKEYCODE_ALT_LEFT)\
        X(LeftSuper,    AKEYCODE_META_LEFT)\
        X(RightShift,   AKEYCODE_SHIFT_RIGHT)\
        X(RightControl,  AKEYCODE_CTRL_RIGHT)\
        X(RightAlt, AKEYCODE_ALT_RIGHT)\
        X(RightSuper,   AKEYCODE_META_RIGHT)\
        \
        X(NumPad0, AKEYCODE_NUMPAD_0)\
        X(NumPad1, AKEYCODE_NUMPAD_1)\
        X(NumPad2, AKEYCODE_NUMPAD_2)\
        X(NumPad3, AKEYCODE_NUMPAD_3)\
        X(NumPad4, AKEYCODE_NUMPAD_4)\
        X(NumPad5, AKEYCODE_NUMPAD_5)\
        X(NumPad6, AKEYCODE_NUMPAD_6)\
        X(NumPad7, AKEYCODE_NUMPAD_7)\
        X(NumPad8, AKEYCODE_NUMPAD_8)\
        X(NumPad9, AKEYCODE_NUMPAD_9)\
        X(NumPadEnter, AKEYCODE_NUMPAD_ENTER)\
        X(NumPadAdd, AKEYCODE_NUMPAD_ADD)\
        X(NumPadDivide, AKEYCODE_NUMPAD_DIVIDE)\
        X(NumPadMultiply, AKEYCODE_NUMPAD_MULTIPLY)\
        X(NumPadSubtract, AKEYCODE_NUMPAD_SUBTRACT)\
        X(NumPadDecimal, AKEYCODE_NUMPAD_DOT)\
        X(NumPadEqual,  AKEYCODE_NUMPAD_EQUALS)\
        \
        X(Comma, AKEYCODE_COMMA)\
        X(Period, AKEYCODE_PERIOD)\
        X(Semicolon, AKEYCODE_SEMICOLON)\
        X(Apostrophe, AKEYCODE_APOSTROPHE)\
        X(BracketLeft, AKEYCODE_LEFT_BRACKET)\
        X(BracketRight, AKEYCODE_RIGHT_BRACKET)\
        X(Backslash, AKEYCODE_BACKSLASH)\
        X(Slash, AKEYCODE_SLASH)\
        X(GraveAccent, AKEYCODE_GRAVE)\
        X(Minus, AKEYCODE_MINUS)\
        X(Equal, AKEYCODE_EQUALS)\
        \
        X(Back, return AKEYCODE_BACK)\
        X(Menu, return AKEYCODE_MENU)\
        X(Search,   return AKEYCODE_SEARCH)\
        \
        X(VolumeMute,   AKEYCODE_VOLUME_MUTE)\
        X(VolumeDown,   AKEYCODE_VOLUME_DOWN)\
        X(VolumeUp, AKEYCODE_VOLUME_UP)\
        X(MediaNext,    AKEYCODE_MEDIA_NEXT)\
        X(MediaPrevious, AKEYCODE_MEDIA_PREVIOUS)\
        X(MediaStop,    AKEYCODE_MEDIA_STOP)\
        X(MediaPlayPause, AKEYCODE_MEDIA_PLAY_PAUSE)




#if __ANDROID_API__ >= 11
    #define  KEY_NATIVE_MAP\
        KEY_NATIVE_MAP\
        X(F13, AKEYCODE_F13)\
        X(F14, AKEYCODE_F14)\
        X(F15, AKEYCODE_F15)\
        X(F16, AKEYCODE_F16)\
        X(F17, AKEYCODE_F17)\
        X(F18, AKEYCODE_F18)\
        X(F19, AKEYCODE_F19)\
        X(F20, AKEYCODE_F20)\
        X(F21, AKEYCODE_F21)\
        X(F22, AKEYCODE_F22)\
        X(F23, AKEYCODE_F23)\
        X(F24, AKEYCODE_F24)
#endif