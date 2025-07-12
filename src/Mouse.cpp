#include <core/Mouse.hpp>
#include <core/Window.hpp>
#include <core/Log.hpp>
#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

Mouse::Mouse() 
	: x(0), y(0)
	, dx(0), dy(0)
	, isMouseIn(false)
	, isLocked(false)
	, hasNewRawDelta(false)
	, wheelDeltaCarry(0)
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
		Error("Mouse row data not regesterd");
	}
	#endif
}

auto Mouse::GetPos() const noexcept -> std::pair<int32_t, int32_t>
{
	return {x, y};
}

auto Mouse::GetRawDelta() const noexcept -> std::pair<int32_t, int32_t>
{
    return {dx, dy};
}

auto Mouse::SetPos([[maybe_unused]] int32_t x_, [[maybe_unused]] int32_t y_) -> void
{
	#if defined(WINDOWS_PLT)
	SetCursorPos(x_, y_);
	#endif
}

auto Mouse::ReadRawDelta() noexcept -> std::optional<std::pair<int32_t, int32_t>>
{
	if(hasNewRawDelta) {
		hasNewRawDelta = false;
		auto delta = std::make_pair(dx, dy);
		dx = dy = 0;
		return delta;
	}
	return std::nullopt;
}

auto Mouse::OnMouseMove( int32_t x, int32_t y ) noexcept -> void
{
	std::tie(this->x, this->y) = { x, y };
}


auto Mouse::OnRawDelta( int32_t dx, int32_t dy ) noexcept -> void
{
	std::tie(this->dx, this->dy) = { dx, dy };
	hasNewRawDelta = true;
}

auto Mouse::OnMouseLeave() noexcept -> void
{
	isMouseIn = false;
}

auto Mouse::OnMouseEnter() noexcept -> void
{
	isMouseIn = true;
}

auto Mouse::OnButtonChange(bool left, bool middle, bool right) noexcept -> void
{
	this->left = left;
	this->middle = middle;
	this->right = right;
}

auto Mouse::OnWheelDelta([[maybe_unused]] int32_t delta ) noexcept -> void
{
	this->delta = delta;
	#if defined(WINDOWS_PLT)
	wheelDeltaCarry += delta;
	while(wheelDeltaCarry >= WHEEL_DELTA) wheelDeltaCarry -= WHEEL_DELTA;
	while(wheelDeltaCarry <= -WHEEL_DELTA) wheelDeltaCarry += WHEEL_DELTA;
	#endif
}

auto Mouse::Locked() const -> bool
{
    return isLocked;
}

auto Mouse::Lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
        #if defined(WINDOWS_PLT)
		auto window_handle =  window.Handle();

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
        emscripten_request_pointerlock("#canvas", EM_TRUE);
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

auto Mouse::IsLeftButtonDown() const -> bool
{
    return left;
}

auto Mouse::IsMiddleButtonDown() 	const -> bool
{
	return middle;
}

auto Mouse::IsRightButtonDown() 		const -> bool
{
	return right;
}

auto Mouse::IsLeftButtonUp() const -> bool
{
    return !left;
}

auto Mouse::IsMiddleButtonUp() 	const -> bool
{
	return !middle;
}

auto Mouse::IsRightButtonUp() 		const -> bool
{
	return !right;
}