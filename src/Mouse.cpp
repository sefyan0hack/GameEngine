#include <core/Mouse.hpp>
#include <core/Window.hpp>
#include <core/Log.hpp>
#if defined(WINDOWS_PLT)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

Mouse::Mouse() : x(0), y(0)
{
}

auto Mouse::GetPos() const noexcept -> std::pair<int32_t, int32_t>
{
	return {x, y};
}

auto Mouse::GetPosX() const noexcept -> int32_t
{
	return x;
}
auto Mouse::GetPosY() const noexcept -> int32_t
{
	return y;
}

auto Mouse::SetPos([[maybe_unused]] int32_t x_, [[maybe_unused]] int32_t y_) -> void
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

auto Mouse::LeftIsPressed() const noexcept -> bool 
{
	return leftIsPressed;
}
auto Mouse::RightIsPressed() const noexcept -> bool 
{
	return rightIsPressed;
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

auto Mouse::OnMouseMove( int32_t newx, int32_t newy ) noexcept -> void
{
	x = newx;
	y = newy;

	buffer.push( Mouse::Event( Mouse::Event::Type::Move,*this ) );
	TrimBuffer();
}

auto Mouse::OnMouseLeave() noexcept -> void
{
	isInWindow = false;
	isEntered = false;
	buffer.push( Mouse::Event( Mouse::Event::Type::Leave,*this ) );
	TrimBuffer();
}

auto Mouse::OnMouseEnter() noexcept -> void
{
	isInWindow = true;
	isEntered = true;
	buffer.push( Mouse::Event( Mouse::Event::Type::Enter,*this ) );
	TrimBuffer();
}

auto Mouse::OnRawDelta( int32_t dx, int32_t dy ) noexcept -> void
{
	rawDeltaBuffer.push( { dx,dy } );
	TrimBuffer();
}

auto Mouse::OnLeftPressed() noexcept -> void
{
	leftIsPressed = true;

	buffer.push( Mouse::Event( Mouse::Event::Type::LPress,*this ) );
	TrimBuffer();
}

auto Mouse::OnLeftReleased() noexcept -> void
{
	leftIsPressed = false;

	buffer.push( Mouse::Event( Mouse::Event::Type::LRelease,*this ) );
	TrimBuffer();
}

auto Mouse::OnRightPressed() noexcept -> void
{
	rightIsPressed = true;

	buffer.push( Mouse::Event( Mouse::Event::Type::RPress,*this ) );
	TrimBuffer();
}

auto Mouse::OnRightReleased() noexcept -> void
{
	rightIsPressed = false;

	buffer.push( Mouse::Event( Mouse::Event::Type::RRelease,*this ) );
	TrimBuffer();
}

auto Mouse::OnWheelUp() noexcept -> void
{
	buffer.push( Mouse::Event( Mouse::Event::Type::WheelUp,*this ) );
	TrimBuffer();
}

auto Mouse::OnWheelDown() noexcept -> void
{
	buffer.push( Mouse::Event( Mouse::Event::Type::WheelDown,*this ) );
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


auto Mouse::OnWheelDelta([[maybe_unused]] int32_t delta ) noexcept -> void
{
	#if defined(WINDOWS_PLT)
	wheelDeltaCarry += delta;
	while( wheelDeltaCarry >= WHEEL_DELTA )
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp();
	}
	while( wheelDeltaCarry <= -WHEEL_DELTA )
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown();
	}
	#endif //_WIN32
}

auto Mouse::IsEmpty() const noexcept -> bool 
{
	return buffer.empty(); 
}

Mouse::Event::Event( Mouse::Event::Type type, const Mouse& parent ) noexcept
    : type( type )
	, leftIsPressed( parent.leftIsPressed )
	, rightIsPressed( parent.rightIsPressed )
    , x( parent.x )
	, y( parent.y ) {}

auto Mouse::Event::GetType() const noexcept -> Mouse::Event::Type 
{
	return type;
}
auto Mouse::Event::GetPos() const noexcept -> std::pair<int32_t, int32_t> 
{
	return {x, y};
}
auto Mouse::Event::GetPosX() const noexcept -> int32_t
{
	return x;
}
auto Mouse::Event::GetPosY() const noexcept -> int32_t
{
	return y;
}
auto Mouse::Event::LeftIsPressed() const noexcept -> bool
{
	return leftIsPressed;
}
auto Mouse::Event::RightIsPressed() const noexcept -> bool 
{
	return rightIsPressed;
}


auto Mouse::Lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
        #if defined(WINDOWS_PLT)
		auto hndl = window.WindowHandle();

        while(ShowCursor(false) >= 0);
		RECT clientRect;
        GetClientRect(hndl, &clientRect);
        POINT topLeft = {0, 0};
        ClientToScreen(hndl, &topLeft);

		RECT clipRect = {
            topLeft.x,
            topLeft.y,
            topLeft.x + clientRect.right,
            topLeft.y + clientRect.bottom
        };
        ClipCursor(&clipRect);
        auto centerX = topLeft.x + clientRect.right / 2;
        auto centerY = topLeft.y + clientRect.bottom / 2;
        SetCursorPos(centerX, centerY);

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
        while(ShowCursor(true) >= 0);
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
		case Type::WheelUp:
			return "WheelUp";
		case Type::WheelDown:
			return "WheelDown";
		case Type::Move:
			return "Move";
		case Type::Enter:
			return "Enter";
		case Type::Leave:
			return "Leave";
		default:
			return "EventType??";
	}
}