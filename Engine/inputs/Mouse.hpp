#pragma once

#include <format>
#include <utility>
#include <bitset>
#include <engine_export.h>

class CWindow;
class ENGINE_EXPORT Mouse
{
    friend class APP;
	friend struct std::formatter<Mouse>;

public:
	enum class Button : uint8_t {
		Left, Middle, Right, WheelUp, WheelDown,
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

	Mouse( const Mouse& ) = delete;
	auto operator=(const Mouse&) -> Mouse& = delete;

	Mouse(Mouse&&) = delete;
	auto operator=(Mouse&&) -> Mouse& = delete;

public:
	auto get_pos() const noexcept			-> std::pair<int32_t, int32_t> ;
	auto set_pos(int32_t x, int32_t y)		-> void ;
	auto get_rawdelta() const noexcept		-> std::pair<float, float> ;
	auto locked() const						-> bool ;
	auto lock([[maybe_unused]] const CWindow& window) noexcept -> void ;
	auto unlock() noexcept					-> void ;
	auto clear_state() noexcept				-> void ;
	auto save_prev_state() noexcept			-> void;

	auto is_down(Button btn) const -> bool;
	auto is_up(Button btn) const -> bool;
	auto is_pressed(Button btn) const -> bool;
	auto is_released(Button btn) const -> bool;

private:
	auto button_down(Button btn) noexcept			-> void ;
	auto button_up(Button btn) noexcept			-> void ;
	auto rawdelta(float dx, float dy) noexcept	-> void ;
	auto mouse_moved(int32_t x, int32_t y) noexcept		-> void ;
	auto mouse_entered() noexcept						-> void ;
	auto mouse_leaved() noexcept						-> void ;

public:
	float Sensitivity;

private:
	constexpr static size_t ButtonCoun = std::to_underlying(Button::Unknown) + 1;

	std::bitset<ButtonCoun> m_CurrButtonState;
	std::bitset<ButtonCoun> m_PrevButtonState;

	// int32_t delta;
	int32_t m_X, m_Y;
	float m_Dx, m_Dy;

	bool isMouseIn = false;
	bool isLocked = false;

	
};

#ifdef __cpp_lib_formatters
// custom Mouse Format
template<>
struct std::formatter<Mouse> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Mouse& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "x": {}, "y": {}, "dx": {}, "dy": {}, "isMouseIn": {}, "isLocked": {} }})"
    , obj.m_X, obj.m_Y, obj.m_Dx, obj.m_Dy, obj.isMouseIn, obj.isLocked
  	);
  }
};
#endif