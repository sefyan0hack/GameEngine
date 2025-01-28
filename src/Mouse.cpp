#include <core/Mouse.hpp>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Mouse::Mouse() : x(0), y(0)
{
}

auto Mouse::GetPos() const noexcept -> std::pair<int, int>
{
	return {x, y};
}

auto Mouse::GetPosX() const noexcept -> int
{
	return x;
}
auto Mouse::GetPosY() const noexcept -> int
{
	return y;
}

auto Mouse::SetPos(int x_, int y_) -> void
{
	SetCursorPos(x_, y_);
}

auto Mouse::IsInWindow() const noexcept -> bool
{
	return isInWindow;
}

auto Mouse::IsEntered() const noexcept -> bool
{
    return isEnterd;
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

auto Mouse::OnMouseMove( int newx,int newy ) noexcept -> void
{
	x = newx;
	y = newy;

	buffer.push( Mouse::Event( Mouse::Event::Type::Move,*this ) );
	TrimBuffer();
}

auto Mouse::OnMouseLeave() noexcept -> void
{
	isInWindow = false;
	buffer.push( Mouse::Event( Mouse::Event::Type::Leave,*this ) );
	TrimBuffer();
}

auto Mouse::OnMouseEnter() noexcept -> void
{
	isInWindow = true;
	buffer.push( Mouse::Event( Mouse::Event::Type::Enter,*this ) );
	TrimBuffer();
}

auto Mouse::OnRawDelta( int dx,int dy ) noexcept -> void
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

auto Mouse::OnWheelDelta(int delta ) noexcept -> void
{
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
}

auto Mouse::IsEmpty() const noexcept -> bool 
{
	return buffer.empty(); 
}

auto Mouse::Event::GetType() const noexcept -> Mouse::Event::Type 
{
	return type;
}
auto Mouse::Event::GetPos() const noexcept -> std::pair<int, int> 
{
	return {x, y};
}
auto Mouse::Event::GetPosX() const noexcept -> int
{
	return x;
}
auto Mouse::Event::GetPosY() const noexcept -> int
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