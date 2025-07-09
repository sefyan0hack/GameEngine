#pragma once
#include <core/fmts.hpp>

class CWindow;
class Mouse
{
    friend class APP;
	friend struct std::formatter<Mouse>;

public:
    struct RawDelta
	{
		int16_t x, y;
	};

	class Event
	{
	public:
		friend struct std::formatter<Event>;
		enum class Type { LPress, LRelease, RPress, RRelease, Move, Enter, Leave };
		static auto Type_to_string(Type t) -> const char*;

		Event( Type type, uint16_t x, uint16_t y ) noexcept;
		auto GetType() const noexcept 			-> Type ;
		auto GetPos() const noexcept 			-> std::pair<uint16_t, uint16_t> ;

		Type type;
		uint16_t x;
		uint16_t y;
	};
public:
    Mouse();
	Mouse( const Mouse& ) = delete;
public:
	auto operator=( const Mouse& ) 			-> Mouse& = delete ;
	auto GetPos() const noexcept 			-> std::pair<uint16_t, uint16_t> ;
	auto ReadRawDelta() noexcept 			-> std::optional<RawDelta> ;
	auto Read() noexcept 					-> std::optional<Mouse::Event> ;
	auto GetPosX() const noexcept 			-> uint16_t ;
	auto GetPosY() const noexcept 			-> uint16_t ;
	auto SetPos(uint16_t x_, uint16_t y_)		-> void ;
	auto IsInWindow() const noexcept 		-> bool ;
	auto IsEntered() const noexcept 		-> bool ;
	auto IsEmpty() const noexcept 			-> bool ;
	auto Flush() noexcept 					-> void ;
	auto Locked() const 					-> bool ;
	auto Lock([[maybe_unused]] const CWindow& window) noexcept -> void ;
	auto UnLock() noexcept 					-> void ;
private:
	auto OnMouseMove( uint16_t x, uint16_t y ) noexcept 	-> void ;
	auto OnMouseLeave() noexcept 						-> void ;
	auto OnMouseEnter() noexcept 						-> void ;
	auto OnRawDelta( int16_t dx, int16_t dy ) noexcept 	-> void ;
	auto OnLeftPressed() noexcept 						-> void ;
	auto OnLeftReleased() noexcept 						-> void ;
	auto OnRightPressed() noexcept 						-> void ;
	auto OnRightReleased() noexcept 					-> void ;
	auto TrimBuffer() noexcept 							-> void ;
	auto TrimRawInputBuffer() noexcept 					-> void ;
	auto OnWheelDelta(int16_t delta ) noexcept 			-> void ;
private:
	static constexpr uint16_t bufferSize = 16u;
	uint16_t x;
	uint16_t y;
	bool isInWindow = false;
	bool isEntered = false;
	bool isLocked = false;
	int16_t wheelDeltaCarry = 0;
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
		R"({{"Type": {}, "x": {}, "y": {}}})",
		Mouse::Event::Type_to_string(obj.type), obj.x, obj.y
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
    R"({{ "x": {}, "y": {}, "isInWindow": {}, "isEntered": {}, "wheelDeltaCarry": {}, "buffer": {}, "rawDeltaBuffer": {} }})"
    , obj.x, obj.y, obj.isInWindow, obj.isEntered, obj.wheelDeltaCarry, QueWrapper{obj.buffer}, QueWrapper{obj.rawDeltaBuffer}
  	);
  }
};