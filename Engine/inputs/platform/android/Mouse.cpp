#include "Mouse.hpp"
#include <core/Log.hpp>
#include <graphics/Window.hpp>

Mouse::Mouse() 
	: Sensitivity(0.11f)
	, m_X(0), m_Y(0)
	, m_Dx(0), m_Dy(0)
	, isMouseIn(false)
	, isLocked(false)
{ CTOR_LOG }

auto Mouse::set_pos([[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y) -> void
{}


auto Mouse::lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
		isLocked = true;
    }

}

auto Mouse::unlock() noexcept -> void
{
	if(isLocked){
		isLocked = false;
    }
}
