#include "Window.hpp"
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <unordered_map>

extern auto from_native(const EmscriptenKeyboardEvent* ev) -> Key;

CWindow::~CWindow()
{
}

static auto resize_callback(int32_t eventType, const EmscriptenUiEvent* e, void*) -> EM_BOOL
{
    if(eventType != EMSCRIPTEN_EVENT_RESIZE) EM_FALSE;

    int w{}, h{};
    emscripten_get_canvas_element_size("#canvas", &w, &h);

    emscripten_set_canvas_element_size("#canvas", w, h);
    EventQ::self().push(CWindow::ResizeEvent{ w, h });
    return EM_TRUE;
}

static auto orientation_callback(int32_t eventType, const EmscriptenOrientationChangeEvent* e, void*) -> EM_BOOL
{
    // Delay 50ms to let browser finish layout
    emscripten_async_call([](void*) {
        resize_callback(EMSCRIPTEN_EVENT_RESIZE, nullptr, nullptr);
    }, nullptr, 50);
    return EM_TRUE;
}

static auto keyboard_callback(int32_t eventType, const EmscriptenKeyboardEvent* e, void*) -> EM_BOOL
{
	switch (eventType)
	{
		case EMSCRIPTEN_EVENT_KEYDOWN:
			EventQ::self().push(Keyboard::KeyDownEvent{from_native(e)});
			break;
		case EMSCRIPTEN_EVENT_KEYUP:
			EventQ::self().push(Keyboard::KeyUpEvent{from_native(e)});
			break;
	}
	return EM_TRUE;
}

static auto mouse_callback(int32_t eventType, const EmscriptenMouseEvent* e, void*) -> EM_BOOL
{
    const double dpr = emscripten_get_device_pixel_ratio();

    const int32_t x = static_cast<int32_t>(std::round(e->targetX * dpr));
    const int32_t y = static_cast<int32_t>(std::round(e->targetY * dpr));

    const float dx = static_cast<float>(e->movementX * dpr);
    const float dy = static_cast<float>(e->movementY * dpr);

    auto btn =
        e->button == 0 ? Mouse::Button::Left :
        e->button == 1 ? Mouse::Button::Middle : Mouse::Button::Right;

    switch (eventType) {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            EventQ::self().push(Mouse::ButtonDownEvent{ btn });
            break;

        case EMSCRIPTEN_EVENT_MOUSEUP:
            EventQ::self().push(Mouse::ButtonUpEvent{ btn });
            break;

        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            EventQ::self().push(Mouse::MoveEvent{ x, y });
            EventQ::self().push(Mouse::MovementEvent{ dx, dy });
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
    static std::unordered_map<int32_t, std::pair<int32_t, int32_t>> lastPos;

    const double dpr = emscripten_get_device_pixel_ratio();

    for (int32_t i = 0; i < e->numTouches; ++i) {
        const auto& t = e->touches[i];
        if (!t.isChanged)
            continue;

        const int32_t id = t.identifier;

        const int32_t x = static_cast<int32_t>(std::round(t.targetX * dpr));
        const int32_t y = static_cast<int32_t>(std::round(t.targetY * dpr));

        switch (eventType) {

            case EMSCRIPTEN_EVENT_TOUCHSTART:
                lastPos[id] = { x, y };
                EventQ::self().push(Mouse::EnterEvent{});
                EventQ::self().push(Mouse::MoveEvent{ x, y });
                break;

            case EMSCRIPTEN_EVENT_TOUCHEND:
            case EMSCRIPTEN_EVENT_TOUCHCANCEL:
                lastPos.erase(id);
                EventQ::self().push(Mouse::LeaveEvent{});
                break;

            case EMSCRIPTEN_EVENT_TOUCHMOVE: {
                auto it = lastPos.find(id);
                if (it != lastPos.end()) {
                    const float dx = static_cast<float>(x - it->second.first);
                    const float dy = static_cast<float>(y - it->second.second);
                    EventQ::self().push(Mouse::MovementEvent{ dx, dy });
                }
                lastPos[id] = { x, y };
                EventQ::self().push(Mouse::MoveEvent{ x, y });
                break;
            }
        }
    }

    return EM_TRUE;
}

static auto register_event_callbacks() -> void
{
	emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &keyboard_callback);
	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &keyboard_callback);
	emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &keyboard_callback);

	emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW    , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW  , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mouseenter_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW , nullptr, EM_FALSE, &mouse_callback);
	emscripten_set_mouseleave_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW , nullptr, EM_FALSE, &mouse_callback);

	emscripten_set_touchstart_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &touch_callback);
	emscripten_set_touchmove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &touch_callback);
	emscripten_set_touchend_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &touch_callback);
	emscripten_set_touchcancel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, &touch_callback);

    emscripten_set_orientationchange_callback(nullptr, EM_FALSE, orientation_callback);
    // do not registere on the canvas element ("#canvas")
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, resize_callback);

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

	emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, 
		[](int32_t, const EmscriptenFullscreenChangeEvent* e, void*) -> EM_BOOL {
			if (e->isFullscreen) logg::tace("Enable FullScreen");
            else logg::trace("Exited fullscreen");
			EventQ::self().push(CWindow::ResizeEvent{ e->elementWidth, e->elementHeight});
			return EM_TRUE;
	});

    emscripten_set_beforeunload_callback(
        nullptr,
        [](int32_t eventType, const void*, void*) -> const char* {
            EventQ::self().push(CWindow::QuitEvent{});
            return "";
        }
    );
}

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
{
	auto window_handle = EMSCRIPTEN_EVENT_TARGET_WINDOW;
	auto Surface = "#canvas";

	emscripten_set_window_title(Title);
	register_event_callbacks();

    emscripten_set_canvas_element_size(Surface, Width, Height);
    EventQ::self().push(CWindow::ResizeEvent{ Width, Height });

	return {0 /* display */, window_handle, Surface};
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
    EmscriptenFullscreenChangeEvent status;
    EMSCRIPTEN_RESULT res = emscripten_get_fullscreen_status(&status);

    if (res == EMSCRIPTEN_RESULT_SUCCESS && status.isFullscreen) {
        res = emscripten_exit_fullscreen();
        if (res != EMSCRIPTEN_RESULT_SUCCESS) {
            logg::trace("Failed to exit fullscreen");
        }
    } else {
        res = emscripten_request_fullscreen(m_Surface, EM_FALSE);
        if (res != EMSCRIPTEN_RESULT_SUCCESS) {
            if (res == EMSCRIPTEN_RESULT_DEFERRED) {
                logg::trace("Fullscreen request deferred (should not happen with EM_FALSE)");
            } else {
                logg::trace("Failed to request fullscreen, error code: ", (int)res);
            }
        }
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
	logg::warn("vSync is always enabled in web and no vSync off ");
}

auto CWindow::message_box(const char* title, const char* body) -> bool
{
    return EM_ASM_INT({
        return confirm(UTF8ToString($0) + "\n\n" + UTF8ToString($1));
    }, title, body);
}

auto CWindow::dims() const	-> std::pair<int32_t, int32_t>
{
	int32_t w{}, h{};
    emscripten_get_canvas_element_size(m_Surface, &w, &h);
    return { w, h };
}

auto CWindow::resize(int32_t width, int32_t height) -> void
{
    emscripten_set_canvas_element_size(m_Surface, width, height);
    EventQ::self().push(CWindow::ResizeEvent{width, height});
}