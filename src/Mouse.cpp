#include "Mouse.hpp"
#include <windows.h>

std::pair<int,int> Mouse::GetPos() const noexcept { return { x,y }; }

int Mouse::GetPosX() const noexcept { return x; }

int Mouse::GetPosY() const noexcept { return y; }

void Mouse::SetPos(int x, int y)
{
	ShowCursor(false);
	SetCursorPos(x, y);
}

bool Mouse::IsInWindow() const noexcept { return isInWindow; }

bool Mouse::IsEntered() const noexcept
{
    return isEnterd;
}

bool Mouse::LeftIsPressed() const noexcept { return leftIsPressed; }

bool Mouse::RightIsPressed() const noexcept { return rightIsPressed; }

void Mouse::Flush() noexcept { buffer = std::queue<Event>(); }

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	if( rawDeltaBuffer.empty() )
	{
		return std::nullopt;
	}
	const RawDelta d = rawDeltaBuffer.front();
	rawDeltaBuffer.pop();
	return d;
}

std::optional<Mouse::Event> Mouse::Read() noexcept
{
	if( buffer.size() > 0u )
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	return {};
}

void Mouse::OnMouseMove( int newx,int newy ) noexcept
{
	x = newx;
	y = newy;

	buffer.push( Mouse::Event( Mouse::Event::Type::Move,*this ) );
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push( Mouse::Event( Mouse::Event::Type::Leave,*this ) );
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push( Mouse::Event( Mouse::Event::Type::Enter,*this ) );
	TrimBuffer();
}

void Mouse::OnRawDelta( int dx,int dy ) noexcept
{
	rawDeltaBuffer.push( { dx,dy } );
	TrimBuffer();
}

void Mouse::OnLeftPressed() noexcept
{
	leftIsPressed = true;

	buffer.push( Mouse::Event( Mouse::Event::Type::LPress,*this ) );
	TrimBuffer();
}

void Mouse::OnLeftReleased() noexcept
{
	leftIsPressed = false;

	buffer.push( Mouse::Event( Mouse::Event::Type::LRelease,*this ) );
	TrimBuffer();
}

void Mouse::OnRightPressed() noexcept
{
	rightIsPressed = true;

	buffer.push( Mouse::Event( Mouse::Event::Type::RPress,*this ) );
	TrimBuffer();
}

void Mouse::OnRightReleased() noexcept
{
	rightIsPressed = false;

	buffer.push( Mouse::Event( Mouse::Event::Type::RRelease,*this ) );
	TrimBuffer();
}

void Mouse::OnWheelUp() noexcept
{
	buffer.push( Mouse::Event( Mouse::Event::Type::WheelUp,*this ) );
	TrimBuffer();
}

void Mouse::OnWheelDown() noexcept
{
	buffer.push( Mouse::Event( Mouse::Event::Type::WheelDown,*this ) );
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}

void Mouse::TrimRawInputBuffer() noexcept
{
	while( rawDeltaBuffer.size() > bufferSize )
	{
		rawDeltaBuffer.pop();
	}
}

void Mouse::OnWheelDelta(int delta ) noexcept
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