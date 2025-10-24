#include "Mouse.hpp"

#if defined(WINDOWS_PLT)
#include <windows.h>
#undef near
#undef far
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <core/Window.hpp>
#include <core/Log.hpp>

Mouse::Mouse() 
	: Sensitivity(0.11f)
	, x(0), y(0)
	, dx(0), dy(0)
	, isMouseIn(false)
	, isLocked(false)
{
	#if defined(WINDOWS_PLT)
	// regester mouse raw data
	RAWINPUTDEVICE _rid {
		.usUsagePage = 0x01,
		.usUsage = 0x02,
		.dwFlags = 0,
		.hwndTarget = nullptr,
	};

	if(RegisterRawInputDevices(&_rid, 1, sizeof(_rid)) == false){
		throw CException("Mouse row data not regesterd");
	}
	#endif
}

auto Mouse::get_pos() const noexcept -> std::pair<int32_t, int32_t>
{
	return {x, y};
}

auto Mouse::get_rawdelta() const noexcept -> std::pair<float, float>
{
    return {Sensitivity * dx, Sensitivity * dy};
}

auto Mouse::set_pos([[maybe_unused]] int32_t x_, [[maybe_unused]] int32_t y_) -> void
{
	#if defined(WINDOWS_PLT)
	SetCursorPos(x_, y_);
	#endif
}


auto Mouse::on_mouse_move( int32_t x, int32_t y ) noexcept -> void
{
	std::tie(this->x, this->y) = { x, y };
}


auto Mouse::on_rawdelta( float dx, float dy ) noexcept -> void
{
	std::tie(this->dx, this->dy) = { dx, dy };
}

auto Mouse::on_mouse_leave() noexcept -> void
{
	isMouseIn = false;
}

auto Mouse::on_mouse_enter() noexcept -> void
{
	isMouseIn = true;
}

auto Mouse::on_button_down(Button btn) noexcept	-> void
{
	const auto index = std::to_underlying(btn);
	m_CurrButtonState[index] = true;
}

auto Mouse::on_button_up(Button btn) noexcept		-> void
{
	const auto index = std::to_underlying(btn);
	m_CurrButtonState[index] = false;
}

auto Mouse::locked() const -> bool
{
    return isLocked;
}

auto Mouse::lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
		[[maybe_unused]] auto window_handle =  window.handle();
		[[maybe_unused]] auto surface = window.surface();

        #if defined(WINDOWS_PLT)

        RECT clientRect;
        GetClientRect(window_handle, &clientRect);
        const int centerX = (clientRect.right - clientRect.left) / 2;
        const int centerY = (clientRect.bottom - clientRect.top) / 2;

        POINT center{centerX, centerY};
        ClientToScreen(window_handle, &center);

        SetCursorPos(center.x, center.y);
        
        RECT clipRect;
        GetClientRect(window_handle, &clipRect);
        MapWindowPoints(window_handle, nullptr, reinterpret_cast<POINT*>(&clipRect), 2);
        ClipCursor(&clipRect);

        SetCursor(nullptr);
        ShowCursor(false);

        x = centerX;
        y = centerY;
        #elif defined(WEB_PLT)
        emscripten_request_pointerlock(surface, EM_TRUE);
        #endif

		isLocked = true;
    }

}

auto Mouse::unlock() noexcept -> void
{
	if(isLocked){
        #if defined(WINDOWS_PLT)
		ClipCursor(nullptr);
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
		ShowCursor(true);
        #elif defined(WEB_PLT)
        emscripten_exit_pointerlock();
        #endif

		isLocked = false;
    }
}

auto Mouse::clear_state() noexcept -> void
{
	m_CurrButtonState.reset();
	m_PrevButtonState.reset();
}

auto Mouse::save_prev_state() noexcept -> void { 
    m_PrevButtonState = m_CurrButtonState;
}

auto Mouse::is_down(Button btn) const -> bool
{
	const auto index = std::to_underlying(btn);
	return m_CurrButtonState[index];
}

auto Mouse::is_up(Button btn) const -> bool
{
    const auto index = std::to_underlying(btn);
	return !m_CurrButtonState[index];
}

auto Mouse::is_pressed(Button btn) const -> bool
{
	const auto index = std::to_underlying(btn);
	return m_CurrButtonState[index] && !m_PrevButtonState[index];;
}

auto Mouse::is_released(Button btn) const -> bool
{
    const auto index = std::to_underlying(btn);
	return !m_CurrButtonState[index] && m_PrevButtonState[index];
}