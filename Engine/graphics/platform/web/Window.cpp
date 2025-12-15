#include "Window.hpp"
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>


CWindow::~CWindow()
{
}

static auto is_mobile() -> bool
{
	return (bool) EM_ASM_INT({
		return window.matchMedia('(max-width: 767px)').matches ? 1 : 0;
	});
}

static auto resize_callback(int32_t eventType, const EmscriptenUiEvent* e, void*) -> EM_BOOL
{
	if(eventType == EMSCRIPTEN_EVENT_RESIZE){
		EventQ::self().push(CWindow::ResizeEvent{e->windowInnerWidth, e->windowInnerHeight});
	}

	return EM_TRUE;
}

static auto keyboard_callback(int32_t eventType, const EmscriptenKeyboardEvent* e, void*) -> EM_BOOL
{
    constexpr auto INVALID = std::numeric_limits<uint32_t>::max();

    uint32_t vk = INVALID;

    // Alphanumeric keys
	if (strncmp(e->code, "Key", 3) == 0 && e->code[3] >= 'A' && e->code[3] <= 'Z') {
        vk = DOM_VK_A + (e->code[3] - 'A');
    }
	else if (strncmp(e->code, "Digit", 5) == 0 && e->code[5] >= '0' && e->code[5] <= '9') {
        vk = DOM_VK_0 + (e->code[5] - '0');
    }

	// Function keys (F1-F24)
    else if (strlen(e->code) > 1 && e->code[0] == 'F') {
        int fnum = atoi(e->code + 1);
        if (fnum >= 1 && fnum <= 24)
            vk = DOM_VK_F1 + (fnum - 1);
    }

    // Special keys
    else if (strcmp(e->code, "Backspace") == 0) vk = DOM_VK_BACK_SPACE;
    else if (strcmp(e->code, "Tab") == 0) vk = DOM_VK_TAB;
    else if (strcmp(e->code, "Enter") == 0) vk = DOM_VK_ENTER;
    else if (strcmp(e->code, "ShiftLeft") == 0 || strcmp(e->code, "ShiftRight") == 0) vk = DOM_VK_SHIFT;
    else if (strcmp(e->code, "ControlLeft") == 0 || strcmp(e->code, "ControlRight") == 0) vk = DOM_VK_CONTROL;
    else if (strcmp(e->code, "AltLeft") == 0) vk = DOM_VK_ALT;
	else if (strcmp(e->code, "AltRight") == 0) vk = DOM_VK_ALTGR;
    else if (strcmp(e->code, "Escape") == 0) vk = DOM_VK_ESCAPE;
    else if (strcmp(e->code, "Space") == 0) vk = DOM_VK_SPACE;

	else if (strcmp(e->code, "CapsLock") == 0) vk = DOM_VK_CAPS_LOCK;
	else if (strcmp(e->code, "PrintScreen") == 0) vk = DOM_VK_PRINTSCREEN;
	else if (strcmp(e->code, "ScrollLock") == 0) vk = DOM_VK_SCROLL_LOCK;
	else if (strcmp(e->code, "Pause") == 0) vk = DOM_VK_PAUSE;

	// Navigation keys
	else if (strcmp(e->code, "Insert") == 0) vk = DOM_VK_INSERT;
	else if (strcmp(e->code, "Delete") == 0) vk = DOM_VK_DELETE;
	else if (strcmp(e->code, "Home") == 0) vk = DOM_VK_HOME;
	else if (strcmp(e->code, "End") == 0) vk = DOM_VK_END;
	else if (strcmp(e->code, "PageUp") == 0) vk = DOM_VK_PAGE_UP;
	else if (strcmp(e->code, "PageDown") == 0) vk = DOM_VK_PAGE_DOWN;

	// Arrow keys
	else if (strcmp(e->code, "ArrowLeft") == 0) vk = DOM_VK_LEFT;
	else if (strcmp(e->code, "ArrowRight") == 0) vk = DOM_VK_RIGHT;
	else if (strcmp(e->code, "ArrowUp") == 0) vk = DOM_VK_UP;
	else if (strcmp(e->code, "ArrowDown") == 0) vk = DOM_VK_DOWN;

	// Numpad keys
	else if (strcmp(e->code, "Numpad0") == 0) vk = DOM_VK_NUMPAD0;
	else if (strcmp(e->code, "Numpad1") == 0) vk = DOM_VK_NUMPAD1;
	else if (strcmp(e->code, "Numpad2") == 0) vk = DOM_VK_NUMPAD2;
	else if (strcmp(e->code, "Numpad3") == 0) vk = DOM_VK_NUMPAD3;
	else if (strcmp(e->code, "Numpad4") == 0) vk = DOM_VK_NUMPAD4;
	else if (strcmp(e->code, "Numpad5") == 0) vk = DOM_VK_NUMPAD5;
	else if (strcmp(e->code, "Numpad6") == 0) vk = DOM_VK_NUMPAD6;
	else if (strcmp(e->code, "Numpad7") == 0) vk = DOM_VK_NUMPAD7;
	else if (strcmp(e->code, "Numpad8") == 0) vk = DOM_VK_NUMPAD8;
	else if (strcmp(e->code, "Numpad9") == 0) vk = DOM_VK_NUMPAD9;
	else if (strcmp(e->code, "NumpadDecimal") == 0) vk = DOM_VK_DECIMAL;
	else if (strcmp(e->code, "NumpadDivide") == 0) vk = DOM_VK_DIVIDE;
	else if (strcmp(e->code, "NumpadMultiply") == 0) vk = DOM_VK_MULTIPLY;
	else if (strcmp(e->code, "NumpadSubtract") == 0) vk = DOM_VK_SUBTRACT;
	else if (strcmp(e->code, "NumpadAdd") == 0) vk = DOM_VK_ADD;
	else if (strcmp(e->code, "NumpadEnter") == 0) vk = DOM_VK_ENTER;
	else if (strcmp(e->code, "NumpadEqual") == 0) vk = DOM_VK_EQUALS;

	// Punctuation keys
	else if (strcmp(e->code, "Comma") == 0) vk = DOM_VK_COMMA;
	else if (strcmp(e->code, "Period") == 0) vk = DOM_VK_PERIOD;
	else if (strcmp(e->code, "Semicolon") == 0) vk = DOM_VK_SEMICOLON;
	else if (strcmp(e->code, "Quote") == 0) vk = DOM_VK_QUOTE;
	else if (strcmp(e->code, "BracketLeft") == 0) vk = DOM_VK_OPEN_BRACKET;
	else if (strcmp(e->code, "BracketRight") == 0) vk = DOM_VK_CLOSE_BRACKET;
	else if (strcmp(e->code, "Backslash") == 0) vk = DOM_VK_BACK_SLASH;
	else if (strcmp(e->code, "Slash") == 0) vk = DOM_VK_SLASH;
	else if (strcmp(e->code, "Backquote") == 0) vk = DOM_VK_BACK_QUOTE;
	else if (strcmp(e->code, "Minus") == 0) vk = DOM_VK_HYPHEN_MINUS;
	else if (strcmp(e->code, "Equal") == 0) vk = DOM_VK_EQUALS;

	// Modifier keys
	else if (strcmp(e->code, "MetaLeft") == 0 || strcmp(e->code, "MetaRight") == 0) 
		vk = DOM_VK_META;

	// Browser control keys (DOM Level 3)
	else if (strcmp(e->code, "BrowserBack") == 0) vk = DOM_VK_BROWSER_BACK;
	else if (strcmp(e->code, "BrowserForward") == 0) vk = DOM_VK_BROWSER_FORWARD;
	else if (strcmp(e->code, "BrowserRefresh") == 0) vk = DOM_VK_BROWSER_REFRESH;
	else if (strcmp(e->code, "BrowserStop") == 0) vk = DOM_VK_BROWSER_STOP;
	else if (strcmp(e->code, "BrowserSearch") == 0) vk = DOM_VK_BROWSER_SEARCH;
	else if (strcmp(e->code, "BrowserFavorites") == 0) vk = DOM_VK_BROWSER_FAVORITES;
	else if (strcmp(e->code, "BrowserHome") == 0) vk = DOM_VK_BROWSER_HOME;

	// Media control keys (DOM Level 3)
	else if (strcmp(e->code, "VolumeMute") == 0) vk = DOM_VK_VOLUME_MUTE;
	else if (strcmp(e->code, "VolumeDown") == 0) vk = DOM_VK_VOLUME_DOWN;
	else if (strcmp(e->code, "VolumeUp") == 0) vk =  DOM_VK_VOLUME_UP;
	else if (strcmp(e->code, "MediaTrackNext") == 0) vk = DOM_VK_MEDIA_TRACK_NEXT;
	else if (strcmp(e->code, "MediaTrackPrevious") == 0) vk = DOM_VK_MEDIA_TRACK_PREVIOUS;
	else if (strcmp(e->code, "MediaStop") == 0) vk = DOM_VK_MEDIA_STOP;
	else if (strcmp(e->code, "MediaPlayPause") == 0) vk = DOM_VK_MEDIA_PLAY_PAUSE;

	if (vk != INVALID) {
        Key key = Keyboard::from_native(vk);

		switch (eventType)
		{
			case EMSCRIPTEN_EVENT_KEYDOWN:
				EventQ::self().push(Keyboard::KeyDownEvent{key});
				break;
			case EMSCRIPTEN_EVENT_KEYUP:
				EventQ::self().push(Keyboard::KeyUpEvent{key});
				break;
		}
    }

	return EM_TRUE;
}

static auto mouse_callback( int32_t eventType, const EmscriptenMouseEvent* e, void*) -> EM_BOOL
{
	auto btn = 
		e->button == 0 ? Mouse::Button::Left:
		e->button == 1 ? Mouse::Button::Middle: Mouse::Button::Right;

	switch (eventType) {

        case EMSCRIPTEN_EVENT_MOUSEDOWN:
			EventQ::self().push(Mouse::ButtonDownEvent{btn});
			break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
			EventQ::self().push(Mouse::ButtonUpEvent{btn});
            break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
			EventQ::self().push(Mouse::MoveEvent{e->targetX, e->targetY});
			EventQ::self().push(Mouse::MovementEvent{static_cast<float>(e->movementX), static_cast<float>(e->movementY)});
            break;

        case EMSCRIPTEN_EVENT_MOUSEENTER:
            EventQ::self().push(Mouse::EnterEvent{});
            break;

        case EMSCRIPTEN_EVENT_MOUSELEAVE:
			EventQ::self().push(Mouse::LeaveEvent{});
            break;
    }

    return EM_TRUE;
}

static auto touch_callback(int32_t eventType, const EmscriptenTouchEvent* e, void*) -> EM_BOOL
{
    static std::unordered_map<int32_t, std::pair<int16_t, int16_t>> lastPos;

    EM_ASM({
        Module._windowwidth = window.innerWidth;
        Module._windowheight = window.innerHeight;
    });

    double windowwidth = EM_ASM_DOUBLE(return Module._windowwidth;);
    double windowheight = EM_ASM_DOUBLE(return Module._windowheight;);

    for (int32_t i = 0; i < e->numTouches; ++i) {
        const auto& t = e->touches[i];
        if (!t.isChanged) 
            continue;

        int32_t id = t.identifier;

        double x = static_cast<double>(t.targetX);
        double y = static_cast<double>(t.targetY);

        if (is_mobile()) {

            double normX = x / windowwidth;
            double normY = y / windowheight;
 
            double logical_x = normY * windowwidth;    
            double logical_y = (1.0 - normX) * windowheight; 

            x = logical_x;
            y = logical_y;
            
        }
        
        switch (eventType) {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
				lastPos[id] = { static_cast<int16_t>(x), static_cast<int16_t>(y) };
				EventQ::self().push(Mouse::EnterEvent{});
                break;
				case EMSCRIPTEN_EVENT_TOUCHEND:
				case EMSCRIPTEN_EVENT_TOUCHCANCEL:
				lastPos.erase(id);
				EventQ::self().push(Mouse::LeaveEvent{});
                break;
				case EMSCRIPTEN_EVENT_TOUCHMOVE:{
				auto old = lastPos.find(id);
				if (old != lastPos.end()) {
					int32_t dx = static_cast<int32_t>(x) - old->second.first;
					int32_t dy = static_cast<int32_t>(y) - old->second.second;
					EventQ::self().push(Mouse::MovementEvent{ static_cast<float>(dx), static_cast<float>(dy) });
				}
				lastPos[id] = { static_cast<int16_t>(x), static_cast<int16_t>(y) };
				EventQ::self().push(Mouse::MoveEvent{static_cast<int32_t>(x), static_cast<int32_t>(y)});
			}
            break;
        }
    }

    return EM_TRUE;
}

static auto register_event_callbacks(H_SRF surface) -> void
{
	emscripten_set_keypress_callback(surface, nullptr, EM_FALSE, &keyboard_callback);
	emscripten_set_keydown_callback(surface, nullptr, EM_FALSE, &keyboard_callback);
	emscripten_set_keyup_callback(surface, nullptr, EM_FALSE, &keyboard_callback);

	emscripten_set_mousedown_callback(surface , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mouseup_callback(surface    , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mousemove_callback(surface  , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mouseenter_callback(surface , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mouseleave_callback(surface , nullptr, EM_FALSE, &mouse_callback);

	emscripten_set_touchstart_callback(surface, nullptr, EM_FALSE, &touch_callback);
	emscripten_set_touchmove_callback(surface, nullptr, EM_FALSE, &touch_callback);
	emscripten_set_touchend_callback(surface, nullptr, EM_FALSE, &touch_callback);
	emscripten_set_touchcancel_callback(surface, nullptr, EM_FALSE, &touch_callback);

	// TODO: how to handle focus events?
	// emscripten_set_focus_callback(surface, nullptr, EM_FALSE,
	// 	[](int32_t, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
	// 		CWindow* window = static_cast<CWindow*>(userData);
    // 		if (!window) return EM_FALSE;

	// 		EventQ::self().push(CWindow::SetFocusEvent{window});
	// 		return EM_TRUE;
	// 	}
	// );

	// emscripten_set_blur_callback(surface, nullptr, EM_FALSE,
	// 	[](int32_t, const EmscriptenFocusEvent *, void* userData) -> EM_BOOL {
	// 		CWindow* window = static_cast<CWindow*>(userData);
    // 		if (!window) return EM_FALSE;

	// 		EventQ::self().push(CWindow::LoseFocusEvent{window});
	// 		return EM_TRUE;
	// 	}
	// );

	emscripten_set_fullscreenchange_callback(surface, nullptr, EM_FALSE, 
		[](int32_t eventType, const EmscriptenFullscreenChangeEvent* e, void*) -> EM_BOOL {
			if (e->isFullscreen) debug::log("Enable FullScreen");
			EventQ::self().push(CWindow::ResizeEvent{ e->elementWidth, e->elementHeight});
			return EM_TRUE;
	});
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::pair<H_WIN, H_SRF>
{
	auto window_handle = EMSCRIPTEN_EVENT_TARGET_WINDOW;
	auto Surface = "#canvas";

	emscripten_set_window_title(Title);
	if(!is_mobile())
		emscripten_set_canvas_element_size(Surface, Width, Height);
	EM_ASM({
        var canvas = document.getElementById('canvas');
        if (!canvas) canvas = document.querySelector('#canvas');
        
        canvas.tabIndex = 0;
        
        canvas.addEventListener('click', function() {
            canvas.focus();
        });
        
        canvas.addEventListener('touchstart', function() {
            canvas.focus();
        });
        
        canvas.focus();
	});

	register_event_callbacks(Surface);
	return {window_handle, Surface};
}

auto CWindow::process_messages() -> void
{
}

auto CWindow::show() -> void
{
	// show using js
	m_Visible = true;
}

auto CWindow::hide() -> void
{
	// hide using js
	m_Visible = false;
}

auto CWindow::toggle_fullscreen() -> void
{
	EmscriptenFullscreenChangeEvent fullscrendata;
    EMSCRIPTEN_RESULT ret = emscripten_get_fullscreen_status(&fullscrendata);

	if (!fullscrendata.isFullscreen) {
		ret = emscripten_request_fullscreen(m_Surface, 1);
	} else {
		ret = emscripten_exit_fullscreen();
	}
}

auto CWindow::swap_buffers() const -> void
{
}

auto CWindow::get_title() -> std::string
{
	return emscripten_get_window_title();
}

auto CWindow::set_title(std::string  title) -> void
{
	emscripten_set_window_title(title.c_str());
}

auto CWindow::set_vsync(bool state) -> void
{
	(void)state;
	debug::log("vSync is always enabled in web and no vSync off ");
}


auto CWindow::message_box(const char* title, const char* body) -> bool
{
	return false; // not implimented yet
}