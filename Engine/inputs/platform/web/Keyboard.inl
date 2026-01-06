#include "Keyboard.hpp"
#include <core/Log.hpp>
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

#define  KEY_NATIVE_MAP\
        X(A, DOM_VK_A)\
        X(B, DOM_VK_B)\
        X(C, DOM_VK_C)\
        X(D, DOM_VK_D)\
        X(E, DOM_VK_E)\
        X(F, DOM_VK_F)\
        X(G, DOM_VK_G)\
        X(H, DOM_VK_H)\
        X(I, DOM_VK_I)\
        X(J, DOM_VK_J)\
        X(K, DOM_VK_K)\
        X(L, DOM_VK_L)\
        X(M, DOM_VK_M)\
        X(N, DOM_VK_N)\
        X(O, DOM_VK_O)\
        X(P, DOM_VK_P)\
        X(Q, DOM_VK_Q)\
        X(R, DOM_VK_R)\
        X(S, DOM_VK_S)\
        X(T, DOM_VK_T)\
        X(U, DOM_VK_U)\
        X(V, DOM_VK_V)\
        X(W, DOM_VK_W)\
        X(X, DOM_VK_X)\
        X(Y, DOM_VK_Y)\
        X(Z, DOM_VK_Z)\
        \
        X(Num0, DOM_VK_0)\
        X(Num1, DOM_VK_1)\
        X(Num2, DOM_VK_2)\
        X(Num3, DOM_VK_3)\
        X(Num4, DOM_VK_4)\
        X(Num5, DOM_VK_5)\
        X(Num6, DOM_VK_6)\
        X(Num7, DOM_VK_7)\
        X(Num8, DOM_VK_8)\
        X(Num9, DOM_VK_9)\
        \
        X(F1, DOM_VK_F1)\
        X(F2, DOM_VK_F2)\
        X(F3, DOM_VK_F3)\
        X(F4, DOM_VK_F4)\
        X(F5, DOM_VK_F5)\
        X(F6, DOM_VK_F6)\
        X(F7, DOM_VK_F7)\
        X(F8, DOM_VK_F8)\
        X(F9, DOM_VK_F9)\
        X(F10, DOM_VK_F10)\
        X(F11, DOM_VK_F11)\
        X(F12, DOM_VK_F12)\
        X(F13, DOM_VK_F13)\
        X(F14, DOM_VK_F14)\
        X(F15, DOM_VK_F15)\
        X(F16, DOM_VK_F16)\
        X(F17, DOM_VK_F17)\
        X(F18, DOM_VK_F18)\
        X(F19, DOM_VK_F19)\
        X(F20, DOM_VK_F20)\
        X(F21, DOM_VK_F21)\
        X(F22, DOM_VK_F22)\
        X(F23, DOM_VK_F23)\
        X(F24, DOM_VK_F24)\
        \
        X(Escape, 	     DOM_VK_ESCAPE)\
        X(Enter, 	     DOM_VK_RETURN)\
        X(Tab, 		     DOM_VK_TAB)\
        X(Backspace,	 DOM_VK_BACK_SPACE)\
        X(Space, 	     DOM_VK_SPACE)\
        X(CapsLock, 	 DOM_VK_CAPS_LOCK)\
        X(PrintScreen,	 DOM_VK_PRINTSCREEN)\
        X(ScrollLock, 	 DOM_VK_SCROLL_LOCK)\
        X(Pause, 	     DOM_VK_PAUSE)\
        \
        X(Insert, 	 DOM_VK_INSERT)\
        X(Home, 	 DOM_VK_HOME)\
        X(PageUp, 	 DOM_VK_PAGE_UP)\
        X(Delete, 	 DOM_VK_DELETE)\
        X(End, 		 DOM_VK_END)\
        X(PageDown,  DOM_VK_PAGE_DOWN)\
        \
        X(Left, 	 DOM_VK_LEFT)\
        X(Right, 	 DOM_VK_RIGHT)\
        X(Up, 		 DOM_VK_UP)\
        X(Down, 	 DOM_VK_DOWN)\
        \
        X(LeftShift,  DOM_VK_SHIFT)\
        X(RightShift,  DOM_VK_SHIFT)\
        X(LeftControl,  DOM_VK_CONTROL)\
        X(RightControl,  DOM_VK_CONTROL)\
        X(LeftAlt,  DOM_VK_ALT)\
        X(RightAlt, DOM_VK_ALT)\
        X(LeftSuper,  DOM_VK_META)\
        X(RightSuper,  DOM_VK_META)\
        \
        X(NumPad0, DOM_VK_NUMPAD0)\
        X(NumPad1, DOM_VK_NUMPAD1)\
        X(NumPad2, DOM_VK_NUMPAD2)\
        X(NumPad3, DOM_VK_NUMPAD3)\
        X(NumPad4, DOM_VK_NUMPAD4)\
        X(NumPad5, DOM_VK_NUMPAD5)\
        X(NumPad6, DOM_VK_NUMPAD6)\
        X(NumPad7, DOM_VK_NUMPAD7)\
        X(NumPad8, DOM_VK_NUMPAD8)\
        X(NumPad9, DOM_VK_NUMPAD9)\
        X(NumPadDecimal, DOM_VK_DECIMAL)\
        X(NumPadDivide, DOM_VK_DIVIDE)\
        X(NumPadMultiply, DOM_VK_MULTIPLY)\
        X(NumPadSubtract, DOM_VK_SUBTRACT)\
        X(NumPadAdd, DOM_VK_ADD)\
        X(NumPadEnter, DOM_VK_ENTER)\
        X(NumPadEqual, DOM_VK_EQUALS)\
        \
        X(Comma, DOM_VK_COMMA)\
        X(Period, DOM_VK_PERIOD)\
        X(Semicolon, DOM_VK_SEMICOLON)\
        X(Apostrophe, DOM_VK_QUOTE)\
        X(BracketLeft, DOM_VK_OPEN_BRACKET)\
        X(BracketRight, DOM_VK_CLOSE_BRACKET)\
        X(Backslash, DOM_VK_BACK_SLASH)\
        X(Slash, DOM_VK_SLASH)\
        X(GraveAccent, DOM_VK_BACK_QUOTE)\
        X(Minus, DOM_VK_HYPHEN_MINUS)\
        X(Equal, DOM_VK_EQUALS)\
        \
        X(BrowserBack, DOM_VK_BROWSER_BACK)\
        X(BrowserForward, DOM_VK_BROWSER_FORWARD)\
        X(BrowserRefresh, DOM_VK_BROWSER_REFRESH)\
        X(BrowserStop, DOM_VK_BROWSER_STOP)\
        X(BrowserSearch, DOM_VK_BROWSER_SEARCH)\
        X(BrowserFavorites, DOM_VK_BROWSER_FAVORITES)\
        X(BrowserHome, DOM_VK_BROWSER_HOME)\
        X(VolumeMute, DOM_VK_VOLUME_MUTE)\
        X(VolumeDown, DOM_VK_VOLUME_DOWN)\
        X(VolumeUp, DOM_VK_VOLUME_UP)\
        X(MediaNext, DOM_VK_MEDIA_TRACK_NEXT)\
        X(MediaPrevious, DOM_VK_MEDIA_TRACK_PREVIOUS)\
        X(MediaStop, DOM_VK_MEDIA_STOP)\
        X(MediaPlayPause, DOM_VK_MEDIA_PLAY_PAUSE)
