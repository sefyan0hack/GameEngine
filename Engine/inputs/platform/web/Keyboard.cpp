#include "Keyboard.hpp"
#include <core/Log.hpp>
#include <emscripten/key_codes.h>
#include <emscripten/html5.h>
#include <engine_export.h>
#include <cstring>
#include <cctype>
#include <string_view>

ENGINE_EXPORT auto from_native(const EmscriptenKeyboardEvent* ev) -> Key
{
    if (!ev) return Key::Unknown;

    const char* code_c = ev->code ? ev->code : "";
    const char* key_c  = ev->key  ? ev->key  : "";
    const int location = ev->location; // 0 = standard, 1 = left, 2 = right, 3 = numpad

    auto sv_code = std::string_view(code_c);
    auto sv_key  = std::string_view(key_c);

    auto starts_with = [](std::string_view s, std::string_view p){
        return s.size() >= p.size() && s.substr(0, p.size()) == p;
    };

    if (!sv_code.empty()) {
        if (sv_code == "NumpadEnter") return Key::NumPadEnter;
        if (starts_with(sv_code, "Numpad")) {
            if (sv_code == "Numpad0") return Key::NumPad0;
            if (sv_code == "Numpad1") return Key::NumPad1;
            if (sv_code == "Numpad2") return Key::NumPad2;
            if (sv_code == "Numpad3") return Key::NumPad3;
            if (sv_code == "Numpad4") return Key::NumPad4;
            if (sv_code == "Numpad5") return Key::NumPad5;
            if (sv_code == "Numpad6") return Key::NumPad6;
            if (sv_code == "Numpad7") return Key::NumPad7;
            if (sv_code == "Numpad8") return Key::NumPad8;
            if (sv_code == "Numpad9") return Key::NumPad9;
            if (sv_code == "NumpadDecimal") return Key::NumPadDecimal;
            if (sv_code == "NumpadDivide")  return Key::NumPadDivide;
            if (sv_code == "NumpadMultiply")return Key::NumPadMultiply;
            if (sv_code == "NumpadSubtract")return Key::NumPadSubtract;
            if (sv_code == "NumpadAdd")     return Key::NumPadAdd;
            if (sv_code == "NumpadEnter")     return Key::NumPadEqual;
            if (sv_code == "NumpadEqual")     return Key::NumPadEnter;
        }

        // Enter
        if (sv_code == "Enter") return Key::Enter;

        // Arrow keys
        if (sv_code == "ArrowLeft")  return Key::Left;
        if (sv_code == "ArrowRight") return Key::Right;
        if (sv_code == "ArrowUp")    return Key::Up;
        if (sv_code == "ArrowDown")  return Key::Down;

        // F1 .. F24
        if (sv_code.size() >= 2 && sv_code[0] == 'F') {
            int fnum = 0;
            for (size_t i = 1; i < sv_code.size(); ++i) {
                if (!isdigit((unsigned char)sv_code[i])) { fnum = 0; break; }
                fnum = fnum * 10 + (sv_code[i] - '0');
            }
            if (fnum >= 1 && fnum <= 24) {
                switch (fnum) {
                    case 1: return Key::F1; case 2: return Key::F2; case 3: return Key::F3;
                    case 4: return Key::F4; case 5: return Key::F5; case 6: return Key::F6;
                    case 7: return Key::F7; case 8: return Key::F8; case 9: return Key::F9;
                    case 10: return Key::F10; case 11: return Key::F11; case 12: return Key::F12;
                    case 13: return Key::F13; case 14: return Key::F14; case 15: return Key::F15;
                    case 16: return Key::F16; case 17: return Key::F17; case 18: return Key::F18;
                    case 19: return Key::F19; case 20: return Key::F20; case 21: return Key::F21;
                    case 22: return Key::F22; case 23: return Key::F23; case 24: return Key::F24;
                }
            }
        }

        // Modifiers
        if (sv_code == "ShiftLeft")  return Key::LeftShift;
        if (sv_code == "ShiftRight") return Key::RightShift;
        if (sv_code == "ControlLeft")  return Key::LeftControl;
        if (sv_code == "ControlRight") return Key::RightControl;
        if (sv_code == "AltLeft" || sv_code == "AltGraph")  return Key::LeftAlt;
        if (sv_code == "AltRight") return Key::RightAlt;
        if (sv_code == "MetaLeft")  return Key::LeftSuper;
        if (sv_code == "MetaRight") return Key::RightSuper;

        // Punctuation / OEM names
        if (sv_code == "Comma")        return Key::Comma;
        if (sv_code == "Period")       return Key::Period;
        if (sv_code == "Minus")        return Key::Minus;
        if (sv_code == "Equal")        return Key::Equal;
        if (sv_code == "BracketLeft")  return Key::BracketLeft;
        if (sv_code == "BracketRight") return Key::BracketRight;
        if (sv_code == "Backslash")    return Key::Backslash;
        if (sv_code == "Slash")        return Key::Slash;
        if (sv_code == "Quote")        return Key::Apostrophe;
        if (sv_code == "Semicolon")    return Key::Semicolon;
        if (sv_code == "Backquote" || sv_code == "IntlBackslash") return Key::GraveAccent;

        // Browser control keys (DOM Level 3)
	    if (sv_code == "BrowserBack") return Key::BrowserBack;
        if (sv_code == "BrowserForward")    return Key::BrowserForward;
        if (sv_code == "BrowserRefresh")    return Key::BrowserRefresh;
        if (sv_code == "BrowserStop")   return Key::BrowserStop;
        if (sv_code == "BrowserSearch") return Key::BrowserSearch;
        if (sv_code == "BrowserFavorites")  return Key::BrowserFavorites;
        if (sv_code == "BrowserHome")   return Key::BrowserHome;

        // Media control keys (DOM Level 3)
        if (sv_code == "VolumeMute")    return Key::VolumeMute;
        if (sv_code == "VolumeDown")    return Key::VolumeDown;
        if (sv_code == "VolumeUp")  return Key::VolumeUp;
        if (sv_code == "MediaTrackNext")    return Key::MediaTrackNext;
        if (sv_code == "MediaTrackPrevious") return Key::MediaTrackPrevious;
        if (sv_code == "MediaStop") return Key::MediaStop;
        if (sv_code == "MediaPlayPause")    return Key::MediaPlayPause;
        
    }

    // A-Z, 0-9 key top-row characters
    if (!sv_key.empty()) {
        if (sv_key.size() == 1) {
            unsigned char ch = static_cast<unsigned char>(sv_key[0]);
            if (std::isalpha(ch)) {
                char up = static_cast<char>(std::toupper(ch));
                switch (up) {
                    case 'A': return Key::A; case 'B': return Key::B; case 'C': return Key::C;
                    case 'D': return Key::D; case 'E': return Key::E; case 'F': return Key::F;
                    case 'G': return Key::G; case 'H': return Key::H; case 'I': return Key::I;
                    case 'J': return Key::J; case 'K': return Key::K; case 'L': return Key::L;
                    case 'M': return Key::M; case 'N': return Key::N; case 'O': return Key::O;
                    case 'P': return Key::P; case 'Q': return Key::Q; case 'R': return Key::R;
                    case 'S': return Key::S; case 'T': return Key::T; case 'U': return Key::U;
                    case 'V': return Key::V; case 'W': return Key::W; case 'X': return Key::X;
                    case 'Y': return Key::Y; case 'Z': return Key::Z;
                }
            }

            if (std::isdigit(ch)) {
                switch (ch) {
                    case '0': return Key::Num0; case '1': return Key::Num1; case '2': return Key::Num2;
                    case '3': return Key::Num3; case '4': return Key::Num4; case '5': return Key::Num5;
                    case '6': return Key::Num6; case '7': return Key::Num7; case '8': return Key::Num8;
                    case '9': return Key::Num9;
                }
            }

            // common punctuation fallback from key
            switch (ch) {
                case ',': return Key::Comma;
                case '.': return Key::Period;
                case ';': return Key::Semicolon;
                case '\'': return Key::Apostrophe;
                case '[': return Key::BracketLeft;
                case ']': return Key::BracketRight;
                case '\\': return Key::Backslash;
                case '/': return Key::Slash;
                case '`': return Key::GraveAccent;
                case '-': return Key::Minus;
                case '=': return Key::Equal;
            }
        }

        // nspecial keys
        if (sv_key == "Enter") return Key::Enter;
        if (sv_key == "Backspace") return Key::Backspace;
        if (sv_key == "Tab") return Key::Tab;
        if (sv_key == "Escape") return Key::Escape;
        if (sv_key == " ") return Key::Space; // some browsers use " " for space
        if (sv_key == "Spacebar") return Key::Space; // older browsers
        if (sv_key == "Pause") return Key::Pause;
        if (sv_key == "ScrollLock") return Key::ScrollLock;
        if (sv_key == "PrintScreen") return Key::PrintScreen;
    }

    // Modifiers fallback via location if we still have generic name
    if (!sv_key.empty()) {
        if (sv_key == "Shift" || sv_key == "ShiftLeft" || sv_key == "ShiftRight") {
            return (location == 2) ? Key::RightShift : Key::LeftShift;
        }
        if (sv_key == "Control" || sv_key == "Ctrl" || sv_key == "ControlLeft" || sv_key == "ControlRight") {
            return (location == 2) ? Key::RightControl : Key::LeftControl;
        }
        if (sv_key == "Alt" || sv_key == "AltLeft" || sv_key == "AltRight") {
            return (location == 2) ? Key::RightAlt : Key::LeftAlt;
        }
        if (sv_key == "Meta" || sv_key == "OS") {
            return (location == 2) ? Key::RightSuper : Key::LeftSuper;
        }
    }

    // As a last resort, map common deprecated keyCode values
    if (ev->keyCode) {
        switch (ev->keyCode) {
            case 13: return (location == 3) ? Key::NumPadEnter : Key::Enter;
            case 37: return Key::Left;
            case 38: return Key::Up;
            case 39: return Key::Right;
            case 40: return Key::Down;
            case 46: return Key::Delete;
            case 36: return Key::Home;
            case 35: return Key::End;
        }
    }
    return Key::Unknown;
}