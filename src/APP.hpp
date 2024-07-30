#pragma once
#include "windows.h"
#include "Window.hpp"

class APP
{
public:
    APP();
    ~APP() = default;

    void Run();

protected:
    virtual void Update(float delta) = 0;

protected:
    Window m_Window;
    LARGE_INTEGER start_count, end_count, counts, freq, fps;
};