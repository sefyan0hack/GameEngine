#include <core/Mouse.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
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

auto Mouse::GetPos() const noexcept -> std::pair<int32_t, int32_t>
{
	return {x, y};
}

auto Mouse::GetRawDelta() const noexcept -> std::pair<float, float>
{
    return {Sensitivity * dx, Sensitivity * dy};
}

auto Mouse::SetPos([[maybe_unused]] int32_t x_, [[maybe_unused]] int32_t y_) -> void
{
	#if defined(WINDOWS_PLT)
	SetCursorPos(x_, y_);
	#endif
}


auto Mouse::OnMouseMove( int32_t x, int32_t y ) noexcept -> void
{
	std::tie(this->x, this->y) = { x, y };
}


auto Mouse::OnRawDelta( float dx, float dy ) noexcept -> void
{
	std::tie(this->dx, this->dy) = { dx, dy };
}

auto Mouse::OnMouseLeave() noexcept -> void
{
	isMouseIn = false;
}

auto Mouse::OnMouseEnter() noexcept -> void
{
	isMouseIn = true;
}

auto Mouse::OnButtonDown(Button btn) noexcept	-> void
{
	const auto index = std::to_underlying(btn);
	m_CurrButtonState[index] = true;
}

auto Mouse::OnButtonUp(Button btn) noexcept		-> void
{
	const auto index = std::to_underlying(btn);
	m_CurrButtonState[index] = false;
}

auto Mouse::Locked() const -> bool
{
    return isLocked;
}

auto Mouse::Lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
		[[maybe_unused]] auto window_handle =  window.Handle();
		[[maybe_unused]] auto surface = window.Surface();

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

auto Mouse::UnLock() noexcept -> void
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

auto Mouse::ClearState() noexcept -> void
{
	m_CurrButtonState.reset();
	m_PrevButtonState.reset();
}

auto Mouse::SavePrevState() noexcept -> void { 
    m_PrevButtonState = m_CurrButtonState;
}

auto Mouse::IsDown(Button btn) const -> bool
{
	const auto index = std::to_underlying(btn);
	return m_CurrButtonState[index];
}

auto Mouse::IsUp(Button btn) const -> bool
{
    const auto index = std::to_underlying(btn);
	return !m_CurrButtonState[index];
}

auto Mouse::IsPressed(Button btn) const -> bool
{
	const auto index = std::to_underlying(btn);
	return m_CurrButtonState[index] && !m_PrevButtonState[index];;
}

auto Mouse::IsReleased(Button btn) const -> bool
{
    const auto index = std::to_underlying(btn);
	return !m_CurrButtonState[index] && m_PrevButtonState[index];
}