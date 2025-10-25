
Mouse::Mouse() 
	: Sensitivity(0.11f)
	, x(0), y(0)
	, dx(0), dy(0)
	, isMouseIn(false)
	, isLocked(false)
{
	// regester mouse raw data
	RAWINPUTDEVICE _rid {
		.usUsagePage = 0x01,
		.usUsage = 0x02,
		.dwFlags = 0,
		.hwndTarget = nullptr,
	};

	if(RegisterRawInputDevices(&_rid, 1, sizeof(_rid)) == false){
		throw Exception("Mouse row data not regesterd");
	}
}

auto Mouse::set_pos([[maybe_unused]] int32_t x_, [[maybe_unused]] int32_t y_) -> void
{
	SetCursorPos(x_, y_);
}


auto Mouse::lock([[maybe_unused]] const CWindow& window) noexcept -> void
{
	if(!isLocked){
		[[maybe_unused]] auto window_handle =  window.handle();
		[[maybe_unused]] auto surface = window.surface();

        RECT clientRect;
        GetClientRect(window_handle, &clientRect);
        const int centerX = (clientRect.right - clientRect.left) / 2;
        const int centerY = (clientRect.bottom - clientRect.top) / 2;

        POINT center{centerX, centerY};
        ClientToScreen(window_handle, &center);

        SetCursorPos(center.x, center.y);
        
        RECT clipRect;
        GetClientRect(window_handle, &clipRect);
        MapWindowPoints(window_handle, nullptr, reinterpret_cast<POINT*>(&clipRect), 2);
        ClipCursor(&clipRect);

        SetCursor(nullptr);
        ShowCursor(false);

        x = centerX;
        y = centerY;

		isLocked = true;
    }

}

auto Mouse::unlock() noexcept -> void
{
	if(isLocked){
		ClipCursor(nullptr);
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
		ShowCursor(true);

		isLocked = false;
    }
}
