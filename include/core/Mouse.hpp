#pragma once
#include <core/fmts.hpp>

class CWindow;
class Mouse
{
    friend class CWindow;
	friend struct std::formatter<Mouse>;

public:
    struct RawDelta
	{
		int32_t x,y;
	};

	class Event
	{
	public:
		friend struct std::formatter<Event>;
		enum class Type { LPress, LRelease, RPress, RRelease, WheelUp, WheelDown, Move, Enter, Leave };
		static auto Type_to_string(Type t) -> const char*;
	public:
		Event( Type type,const Mouse& parent ) noexcept;
		auto GetType() const noexcept 			-> Type ;
		auto GetPos() const noexcept 			-> std::pair<int32_t, int32_t> ;
		auto GetPosX() const noexcept			-> int32_t ;
		auto GetPosY() const noexcept 			-> int32_t ;
		auto LeftIsPressed() const noexcept 	-> bool ;
		auto RightIsPressed() const noexcept 	-> bool ;
    private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int32_t x;
		int32_t y;
	};
public:
    Mouse();
	Mouse( const Mouse& ) = delete;
public:
	auto operator=( const Mouse& ) 			-> Mouse& = delete ;
	auto GetPos() const noexcept 			-> std::pair<int32_t, int32_t> ;
	auto ReadRawDelta() noexcept 			-> std::optional<RawDelta> ;
	auto Read() noexcept 					-> std::optional<Mouse::Event> ;
	auto GetPosX() const noexcept 			-> int32_t ;
	auto GetPosY() const noexcept 			-> int32_t ;
	auto SetPos(int32_t x_, int32_t y_)		-> void ;
	auto IsInWindow() const noexcept 		-> bool ;
	auto IsEntered() const noexcept 		-> bool ;
	auto LeftIsPressed() const noexcept 	-> bool ;
	auto RightIsPressed() const noexcept 	-> bool ;
	auto IsEmpty() const noexcept 			-> bool ;
	auto Flush() noexcept 					-> void ;
	auto Lock() noexcept 					-> void ;
	auto UnLock() noexcept 					-> void ;
private:
	auto OnMouseMove( int32_t x,int32_t y ) noexcept 	-> void ;
	auto OnMouseLeave() noexcept 						-> void ;
	auto OnMouseEnter() noexcept 						-> void ;
	auto OnRawDelta( int32_t dx,int32_t dy ) noexcept 	-> void ;
	auto OnLeftPressed() noexcept 						-> void ;
	auto OnLeftReleased() noexcept 						-> void ;
	auto OnRightPressed() noexcept 						-> void ;
	auto OnRightReleased() noexcept 					-> void ;
	auto OnWheelUp() noexcept 							-> void ;
	auto OnWheelDown() noexcept 						-> void ;
	auto TrimBuffer() noexcept 							-> void ;
	auto TrimRawInputBuffer() noexcept 					-> void ;
	auto OnWheelDelta(int32_t delta ) noexcept 			-> void ;
private:
	static constexpr uint32_t bufferSize = 16u;
	int32_t x;
	int32_t y;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	bool isEntered = false;
	bool isLocked = false;
	int32_t wheelDeltaCarry = 0;
	std::queue<Event> buffer;
	std::queue<RawDelta> rawDeltaBuffer;

	FOR_TEST
};


// custom Mouse::Event Format
template<>
struct std::formatter<Mouse::Event> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mouse::Event& obj, std::format_context& context) const {
    return std::format_to(context.out(),
		R"({{"Type": {}, "leftIsPressed": {}, "rightIsPressed":{}, "x": {}, "y": {}}})",
		Mouse::Event::Type_to_string(obj.type), obj.leftIsPressed, obj.rightIsPressed, obj.x, obj.y
  	);
  }
};

// custom Mouse::RawDelta Format
template<>
struct std::formatter<Mouse::RawDelta > {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mouse::RawDelta& obj, std::format_context& context) const {
    return std::format_to(context.out(),
		R"({{"x": {}, "y": {}}})",
		obj.x, obj.y
  	);
  }
};

// custom Mouse Format
template<>
struct std::formatter<Mouse> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mouse& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "x": {}, "y": {}, "leftIsPressed": {}, "rightIsPressed":{}, "isInWindow": {}, "isEntered": {}, "wheelDeltaCarry": {}, "buffer": {}, "rawDeltaBuffer": {} }})"
    , obj.x, obj.y, obj.leftIsPressed, obj.rightIsPressed, obj.isInWindow, obj.isEntered, obj.wheelDeltaCarry, QueWrapper{obj.buffer}, QueWrapper{obj.rawDeltaBuffer}
  	);
  }
};