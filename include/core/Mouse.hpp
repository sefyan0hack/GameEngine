#pragma once
#include <core/fmts.hpp>

class CWindow;
class Mouse
{
    friend class APP;
	friend struct std::formatter<Mouse>;

public:
	enum class Button : uint8_t {
		Left, Middle, Right,
		Unknown
	};

	struct ButtonDownEvent { Button btn; };
	struct ButtonUpEvent { Button btn; };
	struct EnterEvent {};
	struct LeaveEvent {};
	struct MoveEvent { int32_t x, y; };
	struct MovementEvent { float dx, dy; };

private:
    Mouse();

public:
	~Mouse() = default;

	Mouse( const Mouse& ) = delete;
	auto operator=(const Mouse&) -> Mouse& = delete;

	Mouse(Mouse&&) = delete;
	auto operator=(Mouse&&) -> Mouse& = delete;

public:
	auto GetPos() const noexcept			-> std::pair<int32_t, int32_t> ;
	auto GetRawDelta() const noexcept		-> std::pair<float, float> ;
	auto SetPos(int32_t x_, int32_t y_)		-> void ;
	auto Locked() const						-> bool ;
	auto Lock([[maybe_unused]] const CWindow& window) noexcept -> void ;
	auto UnLock() noexcept					-> void ;
	auto ClearState() noexcept				-> void ;
	auto SavePrevState() noexcept			-> void;

	auto IsDown(Button btn) const -> bool;
	auto IsUp(Button btn) const -> bool;
	auto IsPressed(Button btn) const -> bool;
	auto IsReleased(Button btn) const -> bool;

private:
	auto OnButtonDown(Button btn) noexcept			-> void ;
	auto OnButtonUp(Button btn) noexcept			-> void ;
	auto OnMouseMove(int32_t x, int32_t y) noexcept		-> void ;
	auto OnRawDelta(float dx, float dy) noexcept	-> void ;
	auto OnMouseLeave() noexcept						-> void ;
	auto OnMouseEnter() noexcept						-> void ;

public:
	float Sensitivity;

private:
	constexpr static size_t ButtonCoun = std::to_underlying(Button::Unknown) + 1;

	std::bitset<ButtonCoun> m_CurrButtonState;
	std::bitset<ButtonCoun> m_PrevButtonState;

	// int32_t delta;
	int32_t x, y;
	float dx, dy;

	bool isMouseIn = false;
	bool isLocked = false;

	FOR_TEST
};

// custom Mouse Format
template<>
struct std::formatter<Mouse> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mouse& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "x": {}, "y": {}, "dx": {}, "dy": {}, "isMouseIn": {}, "isLocked": {} }})"
    , obj.x, obj.y, obj.dx, obj.dy, obj.isMouseIn, obj.isLocked
  	);
  }
};