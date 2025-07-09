#include <core/Mouse.hpp>
#include <core/Window.hpp>
#include <core/Log.hpp>
#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

Mouse::Mouse() : x(0), y(0)
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

auto Mouse::GetPos() const noexcept -> std::pair<uint16_t, uint16_t>
{
	return {x, y};
}

auto Mouse::GetPosX() const noexcept -> uint16_t
{
	return x;
}
auto Mouse::GetPosY() const noexcept -> uint16_t
{
	return y;
}

auto Mouse::SetPos([[maybe_unused]] uint16_t x_, [[maybe_unused]] uint16_t y_) -> void
{
	#if defined(WINDOWS_PLT)
	SetCursorPos(x_, y_);
	#endif
}

auto Mouse::IsInWindow() const noexcept -> bool
{
	return isInWindow;
}

auto Mouse::IsEntered() const noexcept -> bool
{
    return isEntered;
}


auto Mouse::Flush() noexcept -> void{ buffer = std::queue<Event>(); }

auto Mouse::ReadRawDelta() noexcept -> std::optional<Mouse::RawDelta>
{
	if( rawDeltaBuffer.empty() )
	{
		return std::nullopt;
	}
	const RawDelta d = rawDeltaBuffer.front();
	rawDeltaBuffer.pop();
	return d;
}

auto Mouse::Read() noexcept -> std::optional<Mouse::Event>
{
	if( buffer.size() > 0u )
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	return {};
}

auto Mouse::OnMouseMove( uint16_t newx, uint16_t newy ) noexcept -> void
{
	x = newx;
	y = newy;

	buffer.push( Mouse::Event( Mouse::Event::Type::Move, x, y ) );
	TrimBuffer();
}

auto Mouse::OnMouseLeave() noexcept -> void
{
	isInWindow = false;
	isEntered = false;
	buffer.push( Mouse::Event( Mouse::Event::Type::Leave, x, y ) );
	TrimBuffer();
}

auto Mouse::OnMouseEnter() noexcept -> void
{
	isInWindow = true;
	isEntered = true;
	buffer.push( Mouse::Event( Mouse::Event::Type::Enter, x, y ) );
	TrimBuffer();
}

auto Mouse::OnRawDelta( int16_t dx, int16_t dy ) noexcept -> void
{
	rawDeltaBuffer.push( { dx, dy } );
	TrimBuffer();
}

auto Mouse::OnLeftPressed() noexcept -> void
{
	buffer.push( Mouse::Event( Mouse::Event::Type::LPress, x, y ) );
	TrimBuffer();
}

auto Mouse::OnLeftReleased() noexcept -> void
{
	buffer.push( Mouse::Event( Mouse::Event::Type::LRelease, x, y ) );
	TrimBuffer();
}

auto Mouse::OnRightPressed() noexcept -> void
{

	buffer.push( Mouse::Event( Mouse::Event::Type::RPress, x, y ) );
	TrimBuffer();
}

auto Mouse::OnRightReleased() noexcept -> void
{

	buffer.push( Mouse::Event( Mouse::Event::Type::RRelease, x, y ) );
	TrimBuffer();
}

auto Mouse::TrimBuffer() noexcept -> void
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}

auto Mouse::TrimRawInputBuffer() noexcept -> void
{
	while( rawDeltaBuffer.size() > bufferSize )
	{
		rawDeltaBuffer.pop();
	}
}


auto Mouse::OnWheelDelta([[maybe_unused]] int16_t delta ) noexcept -> void
{
	#if defined(WINDOWS_PLT)
	wheelDeltaCarry += delta;
	while( wheelDeltaCarry >= WHEEL_DELTA )
	{
		wheelDeltaCarry -= WHEEL_DELTA;
	}
	while( wheelDeltaCarry <= -WHEEL_DELTA )
	{
		wheelDeltaCarry += WHEEL_DELTA;
	}
	#endif
}

auto Mouse::IsEmpty() const noexcept -> bool 
{
	return buffer.empty(); 
}

Mouse::Event::Event( Mouse::Event::Type type, uint16_t x, uint16_t y  ) noexcept
    : type(type)
    , x(x)
	, y(y) {}

auto Mouse::Event::GetType() const noexcept -> Mouse::Event::Type 
{
	return type;
}
auto Mouse::Event::GetPos() const noexcept -> std::pair<uint16_t, uint16_t> 
{
	return {x, y};
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
		auto centerX = window.Width() / 2;
        auto centerY = window.Height() / 2;

        // Confine cursor to window
        RECT clipRect;
        GetClientRect(window_handle, &clipRect);
        MapWindowPoints(window_handle, nullptr, reinterpret_cast<POINT*>(&clipRect), 2);
        ClipCursor(&clipRect);

        // Set cursor to center
        SetCursorPos(centerX + clipRect.left, centerY + clipRect.top);

        // Immediately hide cursor
        SetCursor(nullptr);
		ShowCursor(false);

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

auto Mouse::Event::Type_to_string(Type t) -> const char *
{
	using Type = Mouse::Event::Type;
	switch (t)
	{
		case Type::LPress:
			return "LPress";
		case Type::LRelease:
			return "LRelease";
		case Type::RPress:
			return "RPress";
		case Type::RRelease:
			return "RRelease";
		case Type::Move:
			return "Move";
		case Type::Enter:
			return "Enter";
		case Type::Leave:
			return "Leave";
		default:
			std::unreachable();
	}
}