#pragma once
#include <queue>
#include <optional>

class Window;
class Mouse
{
    friend class Window;
private:
    struct RawDelta
	{
		int x,y;
	};
	class Event
	{
	public:
		enum class Type { LPress, LRelease, RPress, RRelease, WheelUp, WheelDown, Move, Enter, Leave, };
	public:
		Event( Type type,const Mouse& parent ) noexcept
        :   type( type ),
			leftIsPressed( parent.leftIsPressed ), rightIsPressed( parent.rightIsPressed ),
            x( parent.x ), y( parent.y ) {}
		auto GetType() const noexcept 			-> Type ;
		auto GetPos() const noexcept 			-> std::pair<int, int> ;
		auto GetPosX() const noexcept			-> int ;
		auto GetPosY() const noexcept 			-> int ;
		auto LeftIsPressed() const noexcept 	-> bool ;
		auto RightIsPressed() const noexcept 	-> bool ;
    private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x;
		int y;
	};
public:
    Mouse();
	Mouse( const Mouse& ) = delete;
public:
	auto operator=( const Mouse& ) 			-> Mouse& = delete ;
	auto GetPos() const noexcept 			-> std::pair<int, int> ;
	auto ReadRawDelta() noexcept 			-> std::optional<RawDelta> ;
	auto Read() noexcept 					-> std::optional<Mouse::Event> ;
	auto GetPosX() const noexcept 			-> int ;
	auto GetPosY() const noexcept 			-> int ;
	auto SetPos(int x_, int y_) 			-> void ;
	auto IsInWindow() const noexcept 		-> bool ;
	auto IsEntered() const noexcept 		-> bool ;
	auto LeftIsPressed() const noexcept 	-> bool ;
	auto RightIsPressed() const noexcept 	-> bool ;
	auto IsEmpty() const noexcept 			-> bool ;
	auto Flush() noexcept 					-> void ;
private:
	auto OnMouseMove( int x,int y ) noexcept 	-> void ;
	auto OnMouseLeave() noexcept 				-> void ;
	auto OnMouseEnter() noexcept 				-> void ;
	auto OnRawDelta( int dx,int dy ) noexcept 	-> void ;
	auto OnLeftPressed() noexcept 				-> void ;
	auto OnLeftReleased() noexcept 				-> void ;
	auto OnRightPressed() noexcept 				-> void ;
	auto OnRightReleased() noexcept 			-> void ;
	auto OnWheelUp() noexcept 					-> void ;
	auto OnWheelDown() noexcept 				-> void ;
	auto TrimBuffer() noexcept 					-> void ;
	auto TrimRawInputBuffer() noexcept 			-> void ;
	auto OnWheelDelta(int delta ) noexcept 		-> void ;
private:
	static constexpr unsigned int bufferSize = 16u;
	int x;
	int y;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	bool isEnterd = false;
	int wheelDeltaCarry = 0;
	std::queue<Event> buffer;
	std::queue<RawDelta> rawDeltaBuffer;
};