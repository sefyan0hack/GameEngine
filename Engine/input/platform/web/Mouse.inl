
Mouse::Mouse() 
	: Sensitivity(0.11f)
	, x(0), y(0)
	, dx(0), dy(0)
	, isMouseIn(false)
	, isLocked(false)
{}

auto Mouse::set_pos([[maybe_unused]] int32_t x_, [[maybe_unused]] int32_t y_) -> void
{}


auto Mouse::lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
        emscripten_request_pointerlock(window.surface(), EM_TRUE);

		isLocked = true;
    }

}

auto Mouse::unlock() noexcept -> void
{
	if(isLocked){
        emscripten_exit_pointerlock();
		isLocked = false;
    }
}
