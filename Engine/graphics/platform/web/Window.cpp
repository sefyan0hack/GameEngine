#include "Window.hpp"
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>
#include <core/Exception.hpp>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

extern auto from_native(const EmscriptenKeyboardEvent* ev) -> Key;

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

	double css_w, css_h;
    emscripten_get_element_css_size("#canvas", &css_w, &css_h); // for now
    
    double dpr = emscripten_get_device_pixel_ratio();
    
    int32_t pixel_w = static_cast<int32_t>(css_w * dpr);
    int32_t pixel_h = static_cast<int32_t>(css_h * dpr);
    emscripten_set_canvas_element_size("#canvas", pixel_w, pixel_h);

    EventQ::self().push(CWindow::ResizeEvent{pixel_w, pixel_h});
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

auto CWindow::new_window(int32_t Width, int32_t Height, const char* Title) -> std::tuple<H_DSP, H_WIN, H_SRF>
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
	return true;
	//use alert maybe
}

auto CWindow::dims() const	-> std::pair<int32_t, int32_t>
{
	int32_t w, h;
    emscripten_get_canvas_element_size(m_Surface, &w, &h);
    return { w, h };
}

auto CWindow::resize(int32_t width, int32_t height) -> void
{
    EM_ASM({
		var selector = UTF8ToString($0);
        var canvas = document.querySelector(selector)
        if (canvas) {
            canvas.style.width = $1 + 'px';
            canvas.style.height = $2 + 'px';
        } else {
            console.error("Could not find canvas with selector: " + selector);
        }
    }, m_Surface, width, height);

    double dpr = emscripten_get_device_pixel_ratio();
    
    int32_t internal_w = static_cast<int32_t>(width * dpr);
    int32_t internal_h = static_cast<int32_t>(height * dpr);

    emscripten_set_canvas_element_size(m_Surface, internal_w, internal_h);

    EventQ::self().push(CWindow::ResizeEvent{internal_w, internal_h});

    debug::log("Resized: CSS(%dx%d) Internal(%dx%d)", width, height, internal_w, internal_h);
}