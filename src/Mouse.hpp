#pragma once
#include <queue>
#include <optional>

class Window;
class Mouse
{
    friend class Window;
public:
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
		Type GetType() const noexcept { return type; }
		std::pair<int,int> GetPos() const noexcept { return {x, y}; }
		int GetPosX() const noexcept {return x; }
		int GetPosY() const noexcept { return y; }
		bool LeftIsPressed() const noexcept { return leftIsPressed; }
		bool RightIsPressed() const noexcept { return rightIsPressed; }
    private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x;
		int y;
	};
public:
    Mouse() = default;
	Mouse( const Mouse& ) = delete;
public:
	Mouse& operator=( const Mouse& ) = delete;
	std::pair<int,int> GetPos() const noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	void SetPos(int x, int y);
	bool IsInWindow() const noexcept;
	bool IsEntered() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	std::optional<Mouse::Event> Read() noexcept;
	bool IsEmpty() const noexcept { return buffer.empty(); }
	void Flush() noexcept;
private:
	void OnMouseMove( int x,int y ) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnRawDelta( int dx,int dy ) noexcept;
	void OnLeftPressed() noexcept;
	void OnLeftReleased() noexcept;
	void OnRightPressed() noexcept;
	void OnRightReleased() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;
	void OnWheelDelta(int delta ) noexcept;
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